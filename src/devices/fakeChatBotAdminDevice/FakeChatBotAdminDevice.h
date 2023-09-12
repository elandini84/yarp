/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FAKE_CHATBOTADMINDEVICE_H
#define FAKE_CHATBOTADMINDEVICE_H

#include <yarp/dev/IChatBotAdmin.h>
#include <vector>
#include <yarp/dev/DeviceDriver.h>
#include <map>

/**
* @ingroup dev_impl_fake dev_impl_other
*
* @brief `fakeChatBotAdminDevice` : a fake device which implements the IChatBotAdmin interface for testing purposes.
*
*/
class FakeChatBotAdminDevice : public yarp::dev::IChatBotAdmin,
                          public yarp::dev::DeviceDriver
{

public:
    FakeChatBotAdminDevice();
    bool backupBot(std::string& backupString) override;
    bool restoreBot(const std::string& botToRestore) override;

private:
    std::string m_currBot;
    std::map<std::string, std::map<std::string,std::string>> m_qAndA;
};

#endif // FAKE_CHATBOTADMINDEVICE_H
