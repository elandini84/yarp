/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <return_get_all_areas.h>

// Constructor with field values
return_get_all_areas::return_get_all_areas(const yarp::dev::ReturnValue& retval,
                                           const std::vector<yarp::dev::Nav2D::Map2DArea>& areas) :
        WirePortable(),
        retval(retval),
        areas(areas)
{
}

// Read structure on a Wire
bool return_get_all_areas::read(yarp::os::idl::WireReader& reader)
{
    if (!nested_read_retval(reader)) {
        return false;
    }
    if (!read_areas(reader)) {
        return false;
    }
    if (reader.isError()) {
        return false;
    }
    return true;
}

// Read structure on a Connection
bool return_get_all_areas::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(2)) {
        return false;
    }
    if (!read(reader)) {
        return false;
    }
    return true;
}

// Write structure on a Wire
bool return_get_all_areas::write(const yarp::os::idl::WireWriter& writer) const
{
    if (!nested_write_retval(writer)) {
        return false;
    }
    if (!write_areas(writer)) {
        return false;
    }
    if (writer.isError()) {
        return false;
    }
    return true;
}

// Write structure on a Connection
bool return_get_all_areas::write(yarp::os::ConnectionWriter& connection) const
{
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(2)) {
        return false;
    }
    if (!write(writer)) {
        return false;
    }
    return true;
}

// Convert to a printable string
std::string return_get_all_areas::toString() const
{
    yarp::os::Bottle b;
    if (!yarp::os::Portable::copyPortable(*this, b)) {
        return {};
    }
    return b.toString();
}

// read retval field
bool return_get_all_areas::read_retval(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.read(retval)) {
        reader.fail();
        return false;
    }
    return true;
}

// write retval field
bool return_get_all_areas::write_retval(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.write(retval)) {
        return false;
    }
    return true;
}

// read (nested) retval field
bool return_get_all_areas::nested_read_retval(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readNested(retval)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) retval field
bool return_get_all_areas::nested_write_retval(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeNested(retval)) {
        return false;
    }
    return true;
}

// read areas field
bool return_get_all_areas::read_areas(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    size_t _csize;
    yarp::os::idl::WireState _etype;
    reader.readListBegin(_etype, _csize);
    // WireReader removes BOTTLE_TAG_LIST from the tag
    constexpr int expected_tag = ((BOTTLE_TAG_LIST) & (~BOTTLE_TAG_LIST));
    if constexpr (expected_tag != 0) {
        if (_csize != 0 && _etype.code != expected_tag) {
            return false;
        }
    }
    areas.resize(_csize);
    for (size_t _i = 0; _i < _csize; ++_i) {
        if (reader.noMore()) {
            reader.fail();
            return false;
        }
        if (!reader.readNested(areas[_i])) {
            reader.fail();
            return false;
        }
    }
    reader.readListEnd();
    return true;
}

// write areas field
bool return_get_all_areas::write_areas(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, areas.size())) {
        return false;
    }
    for (const auto& _item : areas) {
        if (!writer.writeNested(_item)) {
            return false;
        }
    }
    if (!writer.writeListEnd()) {
        return false;
    }
    return true;
}

// read (nested) areas field
bool return_get_all_areas::nested_read_areas(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    size_t _csize;
    yarp::os::idl::WireState _etype;
    reader.readListBegin(_etype, _csize);
    // WireReader removes BOTTLE_TAG_LIST from the tag
    constexpr int expected_tag = ((BOTTLE_TAG_LIST) & (~BOTTLE_TAG_LIST));
    if constexpr (expected_tag != 0) {
        if (_csize != 0 && _etype.code != expected_tag) {
            return false;
        }
    }
    areas.resize(_csize);
    for (size_t _i = 0; _i < _csize; ++_i) {
        if (reader.noMore()) {
            reader.fail();
            return false;
        }
        if (!reader.readNested(areas[_i])) {
            reader.fail();
            return false;
        }
    }
    reader.readListEnd();
    return true;
}

// write (nested) areas field
bool return_get_all_areas::nested_write_areas(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, areas.size())) {
        return false;
    }
    for (const auto& _item : areas) {
        if (!writer.writeNested(_item)) {
            return false;
        }
    }
    if (!writer.writeListEnd()) {
        return false;
    }
    return true;
}
