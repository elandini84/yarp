/*
* SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
* SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include "Clock_nws_yarp.h"

YARP_LOG_COMPONENT(CLOCK_NWS_YARP, "yarp.devices.clock_nws_yarp")

Clock_nws_yarp::Clock_nws_yarp()
{}


bool Clock_nws_yarp::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(CLOCK_NWS_YARP,"Error! YARP Network is not initialized");
        return false;
    }

    std::string prefix;

    // Check "nws_thrift_port_prefix"
    if (!config.check("nws_thrift_port_prefix", "a prefix for the nws thrift rpc port name"))
    {
        yCWarning(CLOCK_NWS_YARP) << "No 'nws_thrift_port_prefix' parameter found. The thrift port name will be " << m_serverThriftPortName;
    }
    else
    {
        prefix = config.find("nws_thrift_port_prefix").asString();
        if(prefix[0] != '/') {prefix = "/"+prefix;}
        m_serverThriftPortName = prefix + m_serverThriftPortName;
    }

    // Opening m_serverThriftPort
    if(!m_serverThriftPort.open(m_serverThriftPortName))
    {
        yCError(CLOCK_NWS_YARP,"Could not open \"%s\" port",m_serverThriftPortName.c_str());
        return false;
    }
    if(!this->yarp().attachAsServer(m_serverThriftPort))
    {
        yCError(CLOCK_NWS_YARP,"Error! Cannot attach the port as a server");
        return false;
    }

    // Check "streaming_enabled"
    if (config.check("streaming_enabled", "enable/disable the clock publishing on the streaming port")) {
        m_streaming_port_enabled = config.find("streaming_enabled").asInt32() == 1;
    }

    // Opening m_outputStreamPort
    if (m_streaming_port_enabled)
    {
        // Check "output_streaming_port_prefix"
        if (!config.check("output_streaming_port_prefix", "a prefix for the nws thrift rpc port name"))
        {
            yCWarning(CLOCK_NWS_YARP) << "No 'output_streaming_port_prefix' parameter found. The thrift port name will be " << m_outputStreamPortName;
        }
        else
        {
            prefix = config.find("output_streaming_port_prefix").asString();
            if(prefix[0] != '/') {prefix = "/"+prefix;}
            m_outputStreamPortName = prefix + m_outputStreamPortName;
        }

        yCInfo(CLOCK_NWS_YARP) << "Streaming clock on Yarp port enabled";
    }

    yCInfo(CLOCK_NWS_YARP) << "Streaming clock on Yarp port NOT enabled";

    return true;
}


bool Clock_nws_yarp::close()
{
    if(m_serverThriftPort.isOpen())
    {
        m_serverThriftPort.close();
        this->stop();
    }
    return true;
}


bool Clock_nws_yarp::detach()
{
    m_IClock = nullptr;
    return true;
}


bool Clock_nws_yarp::attach( yarp::dev::PolyDriver* deviceToAttach)
{
    deviceToAttach->view(m_IClock);

    if (!m_IClock)
    {
        yCError(CLOCK_NWS_YARP) << "could not attach to the device";
        return false;
    }

    //bool tmp = m_IClock->registerKey(std::to_string(this->getKey()));
    //YARP_UNUSED(tmp);

    if(m_streaming_port_enabled)
    {
        return this->start();
    }

    return true;
}


bool Clock_nws_yarp::threadInit()
{
    if (!m_outputStreamPort.open(m_outputStreamPortName))
    {
        yCError(CLOCK_NWS_YARP, "Could not open \"%s\" port", m_outputStreamPortName.c_str());
        return false;
    }
    yCInfo(CLOCK_NWS_YARP) << "Stream port" << m_outputStreamPortName << "opened on id" << this->getKey();

    return true;
}


void Clock_nws_yarp::threadRelease()
{
    m_outputStreamPort.interrupt();
    m_outputStreamPort.close();
}


void Clock_nws_yarp::run()
{
    bool tmp = m_IClock->registerKey(std::to_string(this->getKey()));
    YARP_UNUSED(tmp);
    while(!this->isStopping())
    {
        if (m_IClock) {
            yarp::dev::ClockData currClock;
            if(!m_IClock->getSynchroClock(std::to_string(this->getKey()),currClock))
            {
                continue;
            }
            m_outputStreamPort.write(currClock);
        }
    }
}

return_getClock Clock_nws_yarp::getClock()
{
    yarp::dev::ClockData clockData(-1,-1);
    return_getClock toReturn;
    if(m_IClock)
    {
        clockData = m_IClock->getClock();
        toReturn.retValue = true;
    }
    else
    {
        toReturn.retValue = false;
        yCError(CLOCK_NWS_YARP) << "No device found. Returning and invalid ClockData object";
    }

    toReturn.returnedClock = clockData;
    return toReturn;
}
