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

FakeClock::~FakeClock() = default;


bool FakeClock::threadInit()
{
    yCTrace(FAKECLOCK);
    return true;
}


void FakeClock::run()
{
    std::lock_guard lockClock(m_clockMutex);
    std::lock_guard lockSynch(m_synchroMutex);
    double gotTime = yarp::os::Time::now();
    m_clockData = convertTime(gotTime);
    std::lock_guard lockKey(m_keyMutex);
    if(!m_readersKeys.empty() && m_readersToGo.empty())
    {
        m_testMutex.unlock();
    }
    m_readersToGo = m_readersKeys;
}


void FakeClock::threadRelease()
{
}


yarp::dev::ClockData FakeClock::convertTime(double input)
{
    yarp::dev::ClockData converted;
    converted.sec = static_cast<int>(input);
    converted.nsec = static_cast<int>((input-static_cast<double>(converted.sec))*1e+9);

    return converted;
}


bool FakeClock::open(yarp::os::Searchable& config)
{
    // check period
    if (!config.check("period", "refresh period of the internally stored clock value"))
    {
        yCInfo(FAKECLOCK) << "Using default 'period' parameter of " << default_period << "s";
    }
    else
    {
        m_period = config.find("period").asFloat64();
    }

    this->setPeriod(m_period);
    return this->start();
}


bool FakeClock::close()
{
    if (this->isRunning())
    {
        this->stop();
    }
    return true;
}


yarp::dev::ClockData FakeClock::getClock()
{
    std::lock_guard lock(m_clockMutex);
    return m_clockData;
}


bool FakeClock::registerKey(std::string inputKey)
{
    std::lock_guard lockKey(m_keyMutex);
    if (std::find(m_readersKeys.begin(), m_readersKeys.end(), inputKey) != m_readersKeys.end())
    {
        yCError(FAKECLOCK) << "The key passed (" << inputKey << ") has already been registered";
        return false;
    }

    m_readersKeys.push_back(inputKey);
    yCInfo(FAKECLOCK) << "KEY REGISTERED (" << inputKey << ")";
    return true;
}


bool FakeClock::getSynchroClock(std::string inputKey, yarp::dev::ClockData& clockToFill)
{
    m_testMutex.lock();
    std::lock_guard lock(m_synchroMutex);
    if (std::find(m_readersToGo.begin(), m_readersToGo.end(), inputKey) == m_readersToGo.end())
    {
        if (!m_readersToGo.empty())
        {
            m_testMutex.unlock();
        }
        return false;
    }
    m_readersToGo.erase(std::remove(m_readersToGo.begin(), m_readersToGo.end(), inputKey), m_readersToGo.end());
    clockToFill.sec = m_clockData.sec;
    clockToFill.nsec = m_clockData.nsec;

    if (!m_readersToGo.empty())
    {
        m_testMutex.unlock();
    }

    return true;
}