# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS OR NOT TARGET YARP::YARP_math)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(CLKS_THRIFT clockService.thrift)

add_library(clockService OBJECT)

target_sources(clockService PRIVATE ${CLKS_THRIFT_GEN_FILES})

target_link_libraries(clockService
  PRIVATE
    YARP::YARP_os
    YARP::YARP_dev
)

target_include_directories(clockService PUBLIC ${CLKS_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET clockService PROPERTY FOLDER "Devices/Shared")
