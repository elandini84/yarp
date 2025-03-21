/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Navigation2D_nwc_yarp.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cmath>

/*! \file Navigation2D_nwc_yarp_iNav2DTarget.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(NAVIGATION2D_NWC_YARP, "yarp.device.navigation2D_nwc_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

ReturnValue Navigation2D_nwc_yarp::gotoTargetByAbsoluteLocation(Map2DLocation loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_nav_RPC.goto_target_by_absolute_location_RPC(loc);
}

ReturnValue Navigation2D_nwc_yarp::gotoTargetByRelativeLocation(double x, double y)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_nav_RPC.goto_target_by_relative_location1_RPC(x,y);
}

ReturnValue Navigation2D_nwc_yarp::gotoTargetByRelativeLocation(double x, double y, double theta)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_nav_RPC.goto_target_by_relative_location2_RPC(x,y,theta);
}

ReturnValue Navigation2D_nwc_yarp::getAbsoluteLocationOfCurrentTarget(Map2DLocation &loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_nav_RPC.get_absolute_location_of_current_target_RPC();
    if (!ret.ret)
    {
        yCError(NAVIGATION2D_NWC_YARP, "Unable to get_absolute_location_of_current_target_RPC");
        return ret.ret;
    }
    loc = ret.loc;
    return ret.ret;
}

ReturnValue Navigation2D_nwc_yarp::getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_nav_RPC.get_relative_location_of_current_target_RPC();
    if (!ret.ret)
    {
        yCError(NAVIGATION2D_NWC_YARP, "Unable to get_relative_location_of_current_target_RPC");
        return ret.ret;
    }
    x = ret.x;
    y = ret.y;
    theta = ret.theta;
    return ret.ret;
}

ReturnValue Navigation2D_nwc_yarp::followPath(const yarp::dev::Nav2D::Map2DPath& locs)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_nav_RPC.follow_path_RPC(locs);
}
