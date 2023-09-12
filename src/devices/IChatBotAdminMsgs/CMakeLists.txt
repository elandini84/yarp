# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(ICHATBOTADMINMSGS_THRIFT IChatBotAdminMsgs.thrift)

add_library(IChatBotAdminMsgs OBJECT)

target_sources(IChatBotAdminMsgs PRIVATE ${ICHATBOTADMINMSGS_THRIFT_GEN_FILES})

target_link_libraries(IChatBotAdminMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(IChatBotAdminMsgs PUBLIC ${ICHATBOTADMINMSGS_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IChatBotAdminMsgs PROPERTY FOLDER "Devices/Shared")
