#!/usr/bin/env python3
#
# SPDX-FileCopyrightText: 2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

"""Generate the catalog consumed by yarprobotinterface-xml-builder.html.

The output JSON intentionally uses the compact schema embedded by the static
HTML page:

  {
    "generated": "YYYY-MM-DD",
    "schema": 1,
    "interfaces": ["IBattery", ...],
    "devices": [
      {
        "n": "deviceName",
        "c": "DeviceClass",
        "p": "src/devices/deviceName",
        "w": "wrapperHint",
        "i": [interface_indexes],
        "q": [attach_target_interface_indexes],
        "x": ["explicitCompatibleDeviceName"],
        "s": 1,
        "m": 1,
        "a": 1,
        "r": [["group", "name", "type", "units", "default", required, "description", "notes"]]
      }
    ]
  }

The generator scans in-tree YARP device plugin declarations from CMake, reads
generated *_ParamsParser.h documentation tables when available, and can also
import third-party device plugin manifests from YARP_DATA_DIRS-style
share/yarp folders.
"""

from __future__ import annotations

import argparse
import datetime as _datetime
import json
import os
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable


EXCLUDED_INTERFACES = {
    "IDeviceDriverParams",
    "IFakeDeviceInterfaceTest1",
    "IFakeDeviceInterfaceTest2",
    "IMPL",
    "IService",
}

WRAPPER_ALIASES = {
    "AudioRecorderWrapper": "audioRecorder_nws_yarp",
    "batteryWrapper": "battery_nws_yarp",
    "analogServer": "multipleanalogsensorsserver",
    "controlboard": "controlBoard_nws_yarp",
    "controlboard_nws_yarp": "controlBoard_nws_yarp",
    "grabber": "audioRecorder_nws_yarp",
    "inertial": "multipleanalogsensorsserver",
    "JoypadControlServer": "JoypadControl_nws_yarp",
    "rgbdSensor_nws_yarp": "RGBDSensor_nws_yarp",
    "serial": "serialPort_nws_yarp",
}

WRAPPER_HELPER_BASES = {
    "WrapperSingle": {"IWrapper", "IMultipleWrapper"},
    "WrapperMultiple": {"IWrapper", "IMultipleWrapper"},
}


@dataclass
class Plugin:
    name: str
    class_name: str = ""
    source_dir: Path | None = None
    rel_path: str = ""
    include: str = ""
    wrapper: str = ""
    external: bool = False
    library: str = ""
    manifest_path: str = ""
    manifest_fields: dict[str, str] = field(default_factory=dict)
    interfaces: set[str] = field(default_factory=set)
    attach_interfaces: set[str] = field(default_factory=set)
    compatible: set[str] = field(default_factory=set)
    can_attach: bool = False
    single_attach: bool = False
    multiple_attach: bool = False
    params: list[list[object]] = field(default_factory=list)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="ignore")


def strip_cpp_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//.*", " ", text)
    return text


def rel_to(root: Path, path: Path) -> str:
    try:
        return path.resolve().relative_to(root.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def clean_table_cell(value: str) -> str:
    value = re.sub(r"\s+", " ", value.strip())
    return "" if value == "-" else value


def truthy_required(value: str) -> int:
    return 0 if value.strip().lower() in {"", "0", "false", "no", "-"} else 1


def parse_base_names(bases: str) -> set[str]:
    names: set[str] = set()
    for base in bases.split(","):
        base = re.sub(r"\b(public|protected|private|virtual)\b", " ", base).strip()
        match = re.match(r"(?:[A-Za-z_]\w*::)*([A-Za-z_]\w*)", base)
        if match:
            names.add(match.group(1))
    return names


def discover_interfaces(repo_root: Path) -> list[str]:
    dev_dir = repo_root / "src/libYARP_dev/src/yarp/dev"
    names: set[str] = set()
    if not dev_dir.is_dir():
        return []
    for header in dev_dir.rglob("*.h"):
        text = strip_cpp_comments(read_text(header))
        names.update(re.findall(r"\bclass\s+(?:YARP_\w+_API\s+)?(I[A-Z]\w+)\b", text))
        names.update(re.findall(r"\busing\s+(I[A-Z]\w+)\s*=", text))
    return sorted(name for name in names if name not in EXCLUDED_INTERFACES)


def parse_class_base_map(paths: Iterable[Path]) -> dict[str, set[str]]:
    classes: dict[str, set[str]] = {}
    pattern = re.compile(
        r"\bclass\s+(?:\w+_API\s+)?(?P<name>(?:[A-Za-z_]\w*::)*[A-Za-z_]\w*)\b(?P<header>[^;{]*?)\{",
        flags=re.S,
    )
    for path in paths:
        text = strip_cpp_comments(read_text(path))
        for match in pattern.finditer(text):
            header = match.group("header")
            if ":" not in header:
                continue
            bases = header.split(":", 1)[1]
            class_name = match.group("name").split("::")[-1]
            classes.setdefault(class_name, set()).update(parse_base_names(bases))
    return classes


def discover_class_base_map(repo_root: Path) -> dict[str, set[str]]:
    paths: list[Path] = []
    for subdir in ["src/libYARP_dev/src/yarp/dev", "src/devices"]:
        path = repo_root / subdir
        if path.is_dir():
            paths.extend(path.rglob("*.h"))
            paths.extend(path.rglob("*.hpp"))
    return parse_class_base_map(paths)


def resolve_base_interfaces(
    name: str,
    class_base_map: dict[str, set[str]],
    interface_names: set[str],
    seen: set[str] | None = None,
) -> set[str]:
    if seen is None:
        seen = set()
    if name in seen:
        return set()
    seen.add(name)
    resolved: set[str] = set()
    if name in interface_names:
        resolved.add(name)
    resolved.update(WRAPPER_HELPER_BASES.get(name, set()))
    for base in class_base_map.get(name, set()):
        resolved.update(resolve_base_interfaces(base, class_base_map, interface_names, seen))
    return resolved


def find_cmake_calls(text: str, function_name: str) -> list[str]:
    calls: list[str] = []
    needle = f"{function_name}("
    start = 0
    while True:
        pos = text.find(needle, start)
        if pos < 0:
            break
        i = pos + len(needle)
        depth = 1
        quote = ""
        escaped = False
        while i < len(text) and depth:
            char = text[i]
            if quote:
                if escaped:
                    escaped = False
                elif char == "\\":
                    escaped = True
                elif char == quote:
                    quote = ""
            else:
                if char in {"'", '"'}:
                    quote = char
                elif char == "(":
                    depth += 1
                elif char == ")":
                    depth -= 1
            i += 1
        if depth == 0:
            calls.append(text[pos + len(needle) : i - 1])
        start = i
    return calls


def tokenize_cmake_args(body: str) -> list[str]:
    body = re.sub(r"#.*", " ", body)
    tokens = re.findall(r'"(?:\\.|[^"])*"|\'(?:\\.|[^\'])*\'|[^\s()]+', body)
    return [token[1:-1] if token[:1] in {"'", '"'} and token[-1:] == token[:1] else token for token in tokens]


def parse_prepare_plugin_call(tokens: list[str], source_dir: Path, repo_root: Path) -> Plugin | None:
    if not tokens:
        return None
    plugin = Plugin(name=tokens[0], source_dir=source_dir, rel_path=rel_to(repo_root, source_dir))
    known_keys = {
        "CATEGORY",
        "TYPE",
        "INCLUDE",
        "DEFAULT",
        "DEPENDS",
        "EXTRA_CONFIG",
        "GENERATE_PARSER",
    }
    i = 1
    category = ""
    while i < len(tokens):
        token = tokens[i]
        upper = token.upper()
        if upper in {"CATEGORY", "TYPE", "INCLUDE", "DEFAULT"} and i + 1 < len(tokens):
            value = tokens[i + 1]
            if upper == "CATEGORY":
                category = value.lower()
            elif upper == "TYPE":
                plugin.class_name = value
            elif upper == "INCLUDE":
                plugin.include = value
            i += 2
            continue
        if upper == "EXTRA_CONFIG":
            i += 1
            while i < len(tokens) and tokens[i].upper() not in known_keys:
                if "=" in tokens[i]:
                    key, value = tokens[i].split("=", 1)
                    if key.lower() == "wrapper":
                        plugin.wrapper = value
                i += 1
            continue
        if "=" in token:
            key, value = token.split("=", 1)
            if key.lower() == "wrapper":
                plugin.wrapper = value
        i += 1
    if category != "device":
        return None
    return plugin


def discover_in_tree_plugins(repo_root: Path) -> list[Plugin]:
    devices_dir = repo_root / "src/devices"
    plugins: list[Plugin] = []
    if not devices_dir.is_dir():
        return plugins
    for cmake in sorted(devices_dir.rglob("CMakeLists.txt")):
        text = read_text(cmake)
        for call in find_cmake_calls(text, "yarp_prepare_plugin"):
            plugin = parse_prepare_plugin_call(tokenize_cmake_args(call), cmake.parent, repo_root)
            if plugin:
                plugins.append(plugin)
    return plugins


def parse_params_table(header_text: str) -> list[list[object]]:
    rows: list[list[object]] = []
    for line in header_text.splitlines():
        match = re.match(r"\s*\*\s*\|(.*)\|\s*$", line)
        if not match:
            continue
        raw_cells = [cell.strip() for cell in match.group(1).split("|")]
        cells = [clean_table_cell(cell) for cell in raw_cells]
        if len(cells) < 8:
            continue
        is_separator = all(cell and set(cell) <= {":", "-"} for cell in raw_cells)
        if cells[0].lower() == "group name" or is_separator:
            continue
        rows.append(
            [
                cells[0],
                cells[1],
                cells[2],
                cells[3],
                cells[4],
                truthy_required(cells[5]),
                cells[6],
                cells[7],
            ]
        )
    return rows


def parser_device_name(header_text: str) -> str:
    match = re.search(r'm_device_name\s*=\s*\{\s*"([^"]+)"\s*\}', header_text)
    return match.group(1) if match else ""


def parser_class_name(header_text: str) -> str:
    match = re.search(r'm_device_classname\s*=\s*\{\s*"([^"]+)"\s*\}', header_text)
    return match.group(1) if match else ""


def find_params_for_plugin(plugin: Plugin) -> list[list[object]]:
    if not plugin.source_dir:
        return []
    candidates = sorted(plugin.source_dir.glob("*_ParamsParser.h"))
    best_by_name: list[list[object]] | None = None
    best_by_class: list[list[object]] | None = None
    fallback: list[list[object]] | None = None
    for candidate in candidates:
        text = read_text(candidate)
        rows = parse_params_table(text)
        if parser_device_name(text) == plugin.name:
            best_by_name = rows
        if plugin.class_name and parser_class_name(text) == plugin.class_name:
            best_by_class = rows
        if len(candidates) == 1:
            fallback = rows
    return best_by_name or best_by_class or fallback or []


def code_for_plugin(plugin: Plugin) -> str:
    if not plugin.source_dir:
        return ""
    paths: list[Path] = []
    if plugin.include:
        include_path = plugin.source_dir / plugin.include
        if include_path.exists():
            paths.append(include_path)
    paths.extend(sorted(plugin.source_dir.glob("*.h")))
    paths.extend(sorted(plugin.source_dir.glob("*.cpp")))
    seen: set[Path] = set()
    parts: list[str] = []
    for path in paths:
        resolved = path.resolve()
        if resolved in seen:
            continue
        seen.add(resolved)
        parts.append(read_text(path))
    return strip_cpp_comments("\n".join(parts))


def class_bases(code: str, class_name: str) -> set[str]:
    if not class_name:
        return set()
    pattern = re.compile(
        r"\bclass\s+(?:\w+_API\s+)?"
        + re.escape(class_name)
        + r"\b(?P<header>[^;{]*?)\{",
        re.S,
    )
    match = pattern.search(code)
    if not match:
        return set()
    header = match.group("header")
    if ":" not in header:
        return set()
    bases = header.split(":", 1)[1]
    return parse_base_names(bases)


def infer_plugin_interfaces(
    plugin: Plugin,
    interface_names: set[str],
    class_base_map: dict[str, set[str]],
) -> None:
    code = code_for_plugin(plugin)
    bases = class_bases(code, plugin.class_name)
    for base in bases:
        plugin.interfaces.update(resolve_base_interfaces(base, class_base_map, interface_names))

    if plugin.interfaces.intersection({"IWrapper", "IMultipleWrapper"}):
        plugin.can_attach = True
        plugin.single_attach = True
    if "IMultipleWrapper" in plugin.interfaces:
        plugin.multiple_attach = True

    if re.search(r"\battach\s*\(\s*(?:yarp::dev::)?PolyDriver\s*\*", code):
        plugin.can_attach = True
        plugin.single_attach = True
        plugin.interfaces.add("IWrapper")
    if re.search(r"\battachAll\s*\(", code) or "PolyDriverList" in code:
        plugin.can_attach = True
        plugin.multiple_attach = True
        plugin.interfaces.add("IMultipleWrapper")
    if re.search(r"\battach\s*\(\s*(?:yarp::dev::)?PolyDriver\s*\*[^)]*,", code):
        plugin.can_attach = True
        plugin.multiple_attach = True
        plugin.interfaces.add("IMultipleWrapper")

    mentioned = set(re.findall(r"\b(I[A-Z]\w+)\b", code))
    mentioned = {name for name in mentioned if name in interface_names}
    if plugin.can_attach:
        plugin.attach_interfaces.update(mentioned - {"IWrapper", "IMultipleWrapper"})


def strip_quotes(value: str) -> str:
    value = value.strip()
    if len(value) >= 2 and value[0] == value[-1] and value[0] in {"'", '"'}:
        return value[1:-1]
    return value


def parse_plugin_manifest(text: str, source_path: str) -> dict[str, object] | None:
    section_name = ""
    fields: dict[str, str] = {}
    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#") or line.startswith(";"):
            continue
        line = re.sub(r"\s+[;#].*$", "", line).strip()
        section = re.match(r"^\[plugin\s+([^\]]+)\]$", line, flags=re.I)
        if section:
            section_name = strip_quotes(section.group(1))
            continue
        pair = re.match(r"^([^\s=]+)(?:\s*=\s*|\s+)(.*)$", line)
        if not pair:
            continue
        fields[pair.group(1).lower()] = strip_quotes(pair.group(2))
    if fields.get("type", "").lower() != "device":
        return None
    name = fields.get("name") or section_name
    if not name:
        return None
    return {
        "name": name,
        "library": fields.get("library", ""),
        "wrapper": fields.get("wrapper", ""),
        "source_path": source_path.replace("\\", "/"),
        "fields": fields,
    }


def parse_interface_list(value: str, interface_names: set[str]) -> set[str]:
    names = re.split(r"[,\s;()]+", strip_quotes(value))
    return {name for name in names if name in interface_names}


def yarp_data_dirs_from_env() -> list[Path]:
    value = os.environ.get("YARP_DATA_DIRS", "")
    return [Path(entry) for entry in value.split(os.pathsep) if entry]


def iter_manifest_paths(data_dirs: Iterable[Path]) -> Iterable[Path]:
    seen: set[Path] = set()
    for data_dir in data_dirs:
        plugins_dir = data_dir / "plugins"
        if not plugins_dir.is_dir():
            continue
        for manifest in sorted(plugins_dir.glob("*.ini")):
            resolved = manifest.resolve()
            if resolved not in seen:
                seen.add(resolved)
                yield manifest


def canonical_wrapper_name(wrapper: str, devices: dict[str, Plugin]) -> str:
    if not wrapper:
        return ""
    if wrapper in devices:
        return wrapper
    alias = WRAPPER_ALIASES.get(wrapper)
    if alias and alias in devices:
        return alias
    lower = wrapper.lower()
    for name in devices:
        if name.lower() == lower:
            return name
    return alias or wrapper


def add_external_manifests(
    plugins: list[Plugin],
    manifests: Iterable[dict[str, object]],
    interface_names: set[str],
) -> None:
    devices = {plugin.name: plugin for plugin in plugins}
    for manifest in manifests:
        name = str(manifest["name"])
        if name in devices:
            continue
        fields = dict(manifest["fields"])  # type: ignore[arg-type]
        plugin = Plugin(
            name=name,
            class_name=str(manifest.get("library") or "external plugin"),
            rel_path=str(manifest.get("source_path") or "external manifest"),
            wrapper=str(manifest.get("wrapper") or ""),
            external=True,
            library=str(manifest.get("library") or ""),
            manifest_path=str(manifest.get("source_path") or ""),
            manifest_fields=fields,
        )
        wrapper_name = canonical_wrapper_name(plugin.wrapper, devices)
        wrapper = devices.get(wrapper_name)
        plugin.interfaces.update(parse_interface_list(fields.get("interfaces", ""), interface_names))
        plugin.interfaces.update(parse_interface_list(fields.get("interface", ""), interface_names))
        if wrapper:
            plugin.interfaces.update(wrapper.attach_interfaces)
        plugin.attach_interfaces.update(parse_interface_list(fields.get("attach_interfaces", ""), interface_names))
        plugin.attach_interfaces.update(parse_interface_list(fields.get("attachinterfaces", ""), interface_names))
        plugin.attach_interfaces.update(parse_interface_list(fields.get("viewed_interfaces", ""), interface_names))
        plugin.attach_interfaces.update(parse_interface_list(fields.get("viewedinterfaces", ""), interface_names))
        if plugin.wrapper and wrapper_name == plugin.name:
            plugin.can_attach = True
            plugin.single_attach = True
            plugin.multiple_attach = True
            plugin.interfaces.update({"IWrapper", "IMultipleWrapper"})
        if plugin.attach_interfaces:
            plugin.can_attach = True
            plugin.single_attach = True
            plugin.multiple_attach = True
            plugin.interfaces.update({"IWrapper", "IMultipleWrapper"})
        plugins.append(plugin)
        devices[plugin.name] = plugin


def link_wrapper_compatibility(plugins: list[Plugin]) -> None:
    devices = {plugin.name: plugin for plugin in plugins}
    for plugin in plugins:
        wrapper_name = canonical_wrapper_name(plugin.wrapper, devices)
        if not wrapper_name or wrapper_name == plugin.name:
            continue
        wrapper = devices.get(wrapper_name)
        if wrapper and wrapper.can_attach:
            wrapper.compatible.add(plugin.name)


def compact_catalog(plugins: list[Plugin], interface_names: Iterable[str]) -> dict[str, object]:
    all_interfaces = set(interface_names)
    for plugin in plugins:
        all_interfaces.update(plugin.interfaces)
        all_interfaces.update(plugin.attach_interfaces)
    interfaces = sorted(name for name in all_interfaces if name and name not in EXCLUDED_INTERFACES)
    iface_index = {name: index for index, name in enumerate(interfaces)}

    devices = []
    for plugin in sorted(plugins, key=lambda item: item.name.lower()):
        entry: dict[str, object] = {
            "n": plugin.name,
            "c": plugin.class_name or plugin.library or "external plugin",
            "p": plugin.rel_path,
            "w": plugin.wrapper,
            "i": [iface_index[name] for name in sorted(plugin.interfaces) if name in iface_index],
            "q": [iface_index[name] for name in sorted(plugin.attach_interfaces) if name in iface_index],
            "x": sorted(plugin.compatible),
            "s": 1 if plugin.single_attach else 0,
            "m": 1 if plugin.multiple_attach else 0,
            "a": 1 if plugin.can_attach else 0,
            "r": plugin.params,
        }
        if plugin.external:
            entry["external"] = True
            entry["manifest"] = {
                "path": plugin.manifest_path,
                "library": plugin.library,
                "fields": plugin.manifest_fields,
            }
        devices.append(entry)

    return {
        "generated": _datetime.date.today().isoformat(),
        "schema": 1,
        "interfaces": interfaces,
        "devices": devices,
    }


def generate_catalog(repo_root: Path, data_dirs: list[Path], include_env: bool) -> dict[str, object]:
    interface_list = discover_interfaces(repo_root)
    interface_names = set(interface_list)
    class_base_map = discover_class_base_map(repo_root)
    plugins = discover_in_tree_plugins(repo_root)
    for plugin in plugins:
        infer_plugin_interfaces(plugin, interface_names, class_base_map)
        plugin.params = find_params_for_plugin(plugin)

    manifest_dirs = []
    if include_env:
        manifest_dirs.extend(yarp_data_dirs_from_env())
    manifest_dirs.extend(data_dirs)
    manifests = []
    for manifest_path in iter_manifest_paths(manifest_dirs):
        parsed = parse_plugin_manifest(read_text(manifest_path), manifest_path.as_posix())
        if parsed:
            manifests.append(parsed)
    add_external_manifests(plugins, manifests, interface_names)
    link_wrapper_compatibility(plugins)
    return compact_catalog(plugins, interface_list)


def update_html_catalog(html_path: Path, catalog: dict[str, object]) -> None:
    html = read_text(html_path)
    payload = json.dumps(catalog, separators=(",", ":"), ensure_ascii=False)
    pattern = re.compile(
        r'(<script\s+id="catalog-data"\s+type="application/json">)(.*?)(</script>)',
        flags=re.S,
    )
    new_html, count = pattern.subn(rf"\1{payload}\3", html, count=1)
    if count != 1:
        raise RuntimeError(f"Could not find catalog-data script tag in {html_path}")
    html_path.write_text(new_html, encoding="utf-8")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--repo-root",
        type=Path,
        default=Path(__file__).resolve().parents[1],
        help="YARP source tree root.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("src/commands/yarprobotinterface/yarprobotinterface-xml-builder-catalog.json"),
        help="Output JSON catalog path, relative to --repo-root unless absolute. Use '-' for stdout.",
    )
    parser.add_argument(
        "--yarp-data-dir",
        type=Path,
        action="append",
        default=[],
        help="Additional share/yarp directory to scan for plugins/*.ini. Can be passed more than once.",
    )
    parser.add_argument(
        "--no-env",
        action="store_true",
        help="Do not scan YARP_DATA_DIRS from the environment.",
    )
    parser.add_argument(
        "--update-html",
        type=Path,
        nargs="?",
        const=Path("src/commands/yarprobotinterface/yarprobotinterface-xml-builder.html"),
        help="Also replace the embedded catalog-data JSON in the static HTML file.",
    )
    parser.add_argument("--pretty", action="store_true", help="Write indented JSON.")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    repo_root = args.repo_root.resolve()
    output = args.output if args.output == Path("-") or args.output.is_absolute() else repo_root / args.output
    html_path = None
    if args.update_html:
        html_path = args.update_html if args.update_html.is_absolute() else repo_root / args.update_html
    data_dirs = [path if path.is_absolute() else repo_root / path for path in args.yarp_data_dir]
    catalog = generate_catalog(repo_root, data_dirs, include_env=not args.no_env)
    json_kwargs = {"ensure_ascii": False}
    if args.pretty:
        json_kwargs.update({"indent": 2, "sort_keys": False})
    else:
        json_kwargs.update({"separators": (",", ":")})
    payload = json.dumps(catalog, **json_kwargs) + "\n"

    if args.output == Path("-"):
        sys.stdout.write(payload)
    else:
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(payload, encoding="utf-8")
    if html_path:
        update_html_catalog(html_path, catalog)
    built_in = sum(1 for device in catalog["devices"] if not device.get("external"))  # type: ignore[index, union-attr]
    external = sum(1 for device in catalog["devices"] if device.get("external"))  # type: ignore[index, union-attr]
    print(
        f"Wrote {len(catalog['interfaces'])} interfaces, {built_in} in-tree devices, {external} external devices",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
