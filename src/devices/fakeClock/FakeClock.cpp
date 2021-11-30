/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include "FakeClock.h"


namespace {
    YARP_LOG_COMPONENT(FAKECLOCK, "yarp.device.FakeClock")
}


FakeClock::FakeClock():
PeriodicThread(default_period)
{
    yCTrace(FAKECLOCK);
}


bool FakeClock::threadInit()
{
    yCTrace(FAKECLOCK);
    return true;
}


void FakeClock::run()
{
    std::lock_guard lock(m_clock_mutex);
    double gotTime = yarp::os::Time::now();

    m_clockData = convertTime(gotTime);
}


void FakeClock::threadRelease()
{
}


bool FakeClock::open(yarp::os::Searchable& config)
{
    // check period
    if (!config.check("period", "refresh period of the internally stored clock value")) {
        yCInfo(FAKECLOCK) << "Using default 'period' parameter of " << default_period << "s";
    }  else {
        m_period = config.find("period").asFloat64();
    }
    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}


bool FakeClock::close()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    return true;
}


yarp::dev::ClockData FakeClock::getClock()
{
    std::lock_guard lock(m_clock_mutex);
    return m_clockData;
}
