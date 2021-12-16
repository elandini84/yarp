# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(fakeClock
  CATEGORY device
  TYPE FakeClock
  INCLUDE FakeClock.h
)

if(NOT SKIP_fakeClock)
  yarp_add_plugin(yarp_fakeClock)

  target_sources(yarp_fakeClock
    PRIVATE
          FakeClock.h
          FakeClock.cpp
    )

  target_link_libraries(yarp_fakeClock
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
      YARP::YARP_sig
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
    YARP::YARP_sig
  )

  yarp_install(
    TARGETS yarp_fakeClock
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeClock PROPERTY FOLDER "Plugins/Device/Fake")
endif()
