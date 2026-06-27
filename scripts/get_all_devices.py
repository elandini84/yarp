import os
from os import path
from pathlib import Path

def camel(s):
    return s[:1].upper() + s[1:] if s else ''

def pascal(s):
    return s[:1].lower() + s[1:] if s else ''

def find_folder(root_folder, target_folder_name):
    root = Path(root_folder)

    for path in root.rglob("*"):
        if path.is_dir() and path.name.lower() == target_folder_name.lower():
            return path

    return None

def find_paramParser(root_folder):
    root = Path(root_folder)

    for path in root.rglob("*"):
        if path.is_file() and "ParamParser" in path.name and path.name.endswith(".h"):
            return path

    return None

def find_deviceName(root_folder,devName):
    root = Path(root_folder)

    for path in root.rglob("*"):
        if path.is_file() and devName.lower() in path.name.lower() and path.name.endswith(".h"):
            return path

    return None

ydd = os.getenv("YARP_DATA_DIRS")

ydd_list = ydd[1:].split(":")
ydd_pl = [x for x in ydd_list if  "/share/yarp" in x]
ydd_list = ydd[1:].split(":")
ydd_pl = [x for x in ydd_pl if  x.endswith("yarp")]
drrs = []
for x in ydd_pl:
    try:
        drrs.append(os.listdir(x))
    except OSError:
        pass
filesInDrrs = []

ct = 0

for l in drrs:
    if "plugins" in l:
        filesInDrrs.append(os.listdir(ydd_pl[ct]+"/plugins"))
        ct += 1

for c in ydd_pl[:-1]:
    for f in filesInDrrs[ydd_pl.index(c)]:
        if not f.startswith("yarp"):
            cont = Path(c+"/plugins/"+f).read_text()
            if "type" in cont and "device" in cont:
                devName = cont.split("name")[1].split("library")[0]
                devName = devName.strip(" ")
                devName = devName.strip("\n")
                print(f"Device name: {devName}")
                dirset = os.listdir("/".join(c.split("/")[:-3])+"/src")
                if "devices" in dirset:
                    devPath = find_folder("/".join(c.split("/")[:-3])+f"/src/devices", devName)
                elif "modules" in dirset:
                    devPath = find_folder("/".join(c.split("/")[:-3])+f"/src/modules", devName)
                if devPath is not None:
                    print(f"Device path: {devPath}")
                    paramParserPath = find_paramParser(devPath)
                    devNamePath = find_deviceName(devPath, devName)
                    if paramParserPath is not None and devNamePath is not None:
                        print(paramParserPath)
                        print(devNamePath)