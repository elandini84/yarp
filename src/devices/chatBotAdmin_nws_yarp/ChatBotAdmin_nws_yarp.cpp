/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ChatBotAdmin_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(CHATBOTADMIN_NWS_YARP, "yarp.devices.chatBotAdmin_nws_yarp")
}

ChatBotAdmin_nws_yarp::~ChatBotAdmin_nws_yarp()
{
    closeMain();
}

bool ChatBotAdmin_nws_yarp::close()
{
    return closeMain();
}

bool ChatBotAdmin_nws_yarp::open(Searchable& prop)
{
    std::string rootName =
        prop.check("name",Value("/chatBotAdmin_nws"),
                    "prefix for port names").asString();

    if (!m_thriftServerPort.open(rootName+"/rpc"))
    {
        yCError(CHATBOTADMIN_NWS_YARP, "Failed to open rpc port");
        return false;
    }
    m_thriftServerPort.setReader(*this);

    yCInfo(CHATBOTADMIN_NWS_YARP, "Device waiting for attach...");
    return true;
}

bool  ChatBotAdmin_nws_yarp::attach(yarp::dev::PolyDriver* deviceToAttach)
{
    if (deviceToAttach->isValid())
    {
        deviceToAttach->view(m_iChatBotAdmin);
    }

    if (nullptr == m_iChatBotAdmin)
    {
        yCError(CHATBOTADMIN_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

    yCInfo(CHATBOTADMIN_NWS_YARP, "Attach done");

    m_msgsImpl.setInterfaces(m_iChatBotAdmin);

    return true;
}

bool  ChatBotAdmin_nws_yarp::detach()
{
    m_iChatBotAdmin = nullptr;
    return true;
}

bool ChatBotAdmin_nws_yarp::closeMain()
{
    //close the port connection here
    m_thriftServerPort.close();
    return true;
}

bool ChatBotAdmin_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    bool b = m_msgsImpl.read(connection);
    if (b)
    {
        return true;
    }
    else
    {
        yCError(CHATBOTADMIN_NWS_YARP, "read() Command failed");
        return false;
    }
}

