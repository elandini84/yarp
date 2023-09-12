/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ChatBotAdmin_nwc_yarp.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

namespace
{
    YARP_LOG_COMPONENT(CHATBOTADMIN_NWC_YARP, "yarp.device.chatBotAdmin_nwc_yarp")
}

bool ChatBotAdmin_nwc_yarp::open(yarp::os::Searchable &config)
{
    std::string local_rpc = config.find("local").asString();
    std::string remote_rpc = config.find("remote").asString();

    if (local_rpc == "")
    {
        yCError(CHATBOTADMIN_NWC_YARP) << "open() error you have to provide a valid 'local' param";
        return false;
    }

    if (remote_rpc == "")
    {
        yCError(CHATBOTADMIN_NWC_YARP) << "open() error you have to provide valid 'remote' param";
        return false;
    }

    if (!m_thriftClientPort.open(local_rpc))
    {
        yCError(CHATBOTADMIN_NWC_YARP) << "Cannot open rpc port, check network";
    }

    bool ok = false;

    ok = yarp::os::Network::connect(local_rpc, remote_rpc);

    if (!ok)
    {
        yCError(CHATBOTADMIN_NWC_YARP) << "open() error could not connect to" << remote_rpc;
        return false;
    }

    if (!m_thriftClient.yarp().attachAsClient(m_thriftClientPort))
    {
        yCError(CHATBOTADMIN_NWC_YARP) << "Cannot attach the m_thriftClientPort port as client";
    }

    yCDebug(CHATBOTADMIN_NWC_YARP) << "Opening of nwc successful";
    return true;
}

bool ChatBotAdmin_nwc_yarp::close()
{
    m_thriftClientPort.close();
    return true;
}

bool ChatBotAdmin_nwc_yarp::backupBot(std::string& backupString)
{
    return_backupBot output = m_thriftClient.backupBotRPC();
    if(!output.result)
    {
        yCError(CHATBOTADMIN_NWC_YARP) << "Could not backup the chatbot";
        return false;
    }
    return true;
}

bool ChatBotAdmin_nwc_yarp::restoreBot(const std::string& botToRestore)
{
    if(!m_thriftClient.restoreBotRPC(botToRestore))
    {
        yCError(CHATBOTADMIN_NWC_YARP) << "Could not restore the chatbot";
        return false;
    }
    return true;
}
