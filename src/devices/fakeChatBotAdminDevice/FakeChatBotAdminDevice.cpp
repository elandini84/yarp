/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeChatBotAdminDevice.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

YARP_LOG_COMPONENT(FAKECHATBOTADMINDEVICE, "yarp.devices.fakeChatBotAdminDevice")

FakeChatBotAdminDevice::FakeChatBotAdminDevice() :
    m_qAndA{
            {"greetings",
             {{"Hello!","Hello there."},{"Who are you?","I am just a fake chatbot"},
              {"Goodbye!", "Already? Ok... bye."}}
            },
            {"chatting",
             {{"Hello!","Hello again."},{"Who are you?","I am just a fake chatbot"},
              {"Goodbye!", "Bye bye!"}}
            }
        }
{}

bool FakeChatBotAdminDevice::backupBot(std::string& backupString)
{
    YARP_UNUSED(backupString);
    backupString = "{";
    for(auto& [status, ctx] : m_qAndA)
    {
        backupString += status;
        backupString += ":\n{\n\t";
        for(auto& [mIn, mOut] : ctx)
        {
            backupString += mIn;
            backupString += ": ";
            backupString += mOut;
            backupString += ",\n";
        }
        backupString += "},\n";
    }
    backupString += "}";

    return true;
}

bool FakeChatBotAdminDevice::restoreBot(const std::string& botToRestore)
{
    YARP_UNUSED(botToRestore);
    yCDebug(FAKECHATBOTADMINDEVICE) << "Not implemented yet";
    return true;
}