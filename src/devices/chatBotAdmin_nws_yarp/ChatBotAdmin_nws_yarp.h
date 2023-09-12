/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CHATBOTADMIN_NWS_YARP_H
#define YARP_DEV_CHATBOTADMIN_NWS_YARP_H

#include <cstdio>
#include <cstdlib>
#include <mutex>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IChatBotAdmin.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RpcServer.h>
#include <yarp/dev/WrapperSingle.h>
#include "IChatBotAdminMsgs.h"

using namespace yarp::os;
using namespace yarp::dev;

// rpc commands
class IChatBotAdminMsgsImpl : public IChatBotAdminMsgs
{
private:
    std::mutex           m_mutex;
    yarp::dev::IChatBotAdmin* m_iChatBotAdmin{nullptr};
    yarp::os::Port*      m_output_port{nullptr};

public:
    return_backupBot   backupBotRPC() override;
    bool               restoreBotRPC(const std::string& botToRestore) override;

public:
    void setInterfaces(yarp::dev::IChatBotAdmin* iser) { m_iChatBotAdmin = iser;}
    void setOutputPort(yarp::os::Port* port) { m_output_port = port; }
    std::mutex* getMutex() { return &m_mutex; }
};


/**
 * @ingroup dev_impl_nws_yarp
 *
 * \brief `ChatBotAdmin_nws_yarp`: A wrapper for a plugin able to backup0 and restore a chatbot.
 *
 * The interface consists of one rpc port. This allows to access to the functions of the IChatBotAdmin interface
 * wrapped by the nws
 *
 * Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value                | Required    | Description                                                           | Notes                                                                               |
 * |:--------------:|:--------------:|:-------:|:--------------:|:----------------------------:|:----------: |:---------------------------------------------------------------------:|:-----------------------------------------------------------------------------------:|
 * | name           |      -         | string  | -              |   /chatBotAdmin_nws          | No          | full name of the port opened by the device                            | MUST start with a '/' character, xxx/text:i, xxx/rpc, xxx/audio:o ports are opened  |
 */
class ChatBotAdmin_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader
{
private:
    yarp::dev::IChatBotAdmin* m_iChatBotAdmin{ nullptr };
    yarp::os::RpcServer   m_thriftServerPort;
    IChatBotAdminMsgsImpl m_msgsImpl;

    // yarp::dev::IWrapper
    bool  attach(yarp::dev::PolyDriver* deviceToAttach) override;
    bool  detach() override;

private:
    bool closeMain();

public:
    ChatBotAdmin_nws_yarp() = default;
    ChatBotAdmin_nws_yarp(const ChatBotAdmin_nws_yarp&) = delete;
    ChatBotAdmin_nws_yarp(ChatBotAdmin_nws_yarp&&) = delete;
    ChatBotAdmin_nws_yarp& operator=(const ChatBotAdmin_nws_yarp&) = delete;
    ChatBotAdmin_nws_yarp& operator=(ChatBotAdmin_nws_yarp&&) = delete;
    virtual ~ChatBotAdmin_nws_yarp() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_CHATBOTADMIN_NWS_YARP_H
