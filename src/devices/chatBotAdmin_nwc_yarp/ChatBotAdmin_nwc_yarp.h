/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CHATBOTADMIN_NWC_YARP_H
#define YARP_DEV_CHATBOTADMIN_NWC_YARP_H

#include <yarp/os/Network.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IChatBotAdmin.h>
#include "IChatBotAdminMsgs.h"

 /**
 * @ingroup dev_impl_network_clients
 *
 * \brief `chatBotAdmin_nwc_yarp`: The client side of any IChatBotAdmin capable device.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -              |   -           | Yes          | Full port name opened by the nwc device.                |       |
 * | remote         |      -         | string  | -              |   -           | Yes          | Full port name of the port opened on the server side, to which the nwc connects to.    |     |
 */
class ChatBotAdmin_nwc_yarp : public yarp::dev::DeviceDriver,
                     public yarp::dev::IChatBotAdmin
{
protected:
    // thrift interface
    IChatBotAdminMsgs m_thriftClient;

    // rpc port
    yarp::os::Port m_thriftClientPort;

public:

    //From DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //From IChatBotAdmin
    bool backupBot(std::string& backupString) override;
    bool restoreBot(const std::string& botToRestore) override;
};

#endif // YARP_DEV_CHATBOTADMIN_NWC_YARP_H
