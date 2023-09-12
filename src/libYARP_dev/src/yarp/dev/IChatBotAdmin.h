/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICHATBOTADMIN_H
#define YARP_DEV_ICHATBOTADMIN_H

#include <yarp/dev/api.h>

#include <string>
#include <vector>


namespace yarp::dev {
class IChatBotAdmin;
} // namespace yarp

/**
 * @ingroup dev_iface_other
 *
 * IChatBot interface. Provides methods to interact with chatbots.
 */
class YARP_dev_API yarp::dev::IChatBotAdmin
{
public:
    /**
     * Destructor
     */
    virtual ~IChatBotAdmin();

    /**
     * Downloads a backup of the chatbot.
     * \param backupString a string (with chatbot implementation dependent formatting) that contains a full backup of the chatbot
     * \return true on success
     */
    virtual bool backupBot(std::string& backupString) = 0;

    /**
     * Restores a previous version of the chatbot
     * \param botToRestore a string (with chatbot implementation dependent formatting) that contains a previously backed up version of the bot
     * \return true on success
     */
    virtual bool restoreBot(const std::string& botToRestore) = 0;

};


#endif //YARP_DEV_ICHATBOTADMIN_H
