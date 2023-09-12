/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ICHATBOTADMINTEST_H
#define ICHATBOTADMINTEST_H

#include <yarp/dev/IChatBotAdmin.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;

namespace yarp::dev::tests {
inline void exec_iChatBotAdmin_test_1(yarp::dev::IChatBotAdmin* ichatbotadmin)
{
    REQUIRE(ichatbotadmin != nullptr);

    bool b;

    std::string tempBackUp;
    b = ichatbotadmin->backupBot(tempBackUp);
    CHECK(b);

    std::string tempBackUp2;
    b = ichatbotadmin->restoreBot(tempBackUp);
    CHECK(b);
    ichatbotadmin->backupBot(tempBackUp2);
    CHECK(tempBackUp2 == tempBackUp);
}
}

#endif //ICHATBOTADMINTEST_H
