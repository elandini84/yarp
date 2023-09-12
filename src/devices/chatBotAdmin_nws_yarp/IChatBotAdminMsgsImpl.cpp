/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ChatBotAdmin_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(ICHATBOTADMINMSGSIMPL, "yarp.devices.chatBotAdmin_nws_yarp.ChatBotAdminRPC_CallbackHelper")
}

return_backupBot IChatBotAdminMsgsImpl::backupBotRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_backupBot response;
    std::string backupString;
    if(!m_iChatBotAdmin->backupBot(backupString))
    {
        yCError(ICHATBOTADMINMSGSIMPL) << "Could not backup the bot";
        response.result = false;
        return response;
    }
    response.backupString = backupString;
    response.result = true;

    return response;
}

bool IChatBotAdminMsgsImpl::restoreBotRPC(const std::string& botToRestore)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    if(!m_iChatBotAdmin->restoreBot(botToRestore))
    {
        yCError(ICHATBOTADMINMSGSIMPL) << "Could not restore the provided bot";

        return false;
    }

    return true;
}
