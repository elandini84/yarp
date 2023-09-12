/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

#include <yarp/sig/Sound.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::chatBotAdmin_nws_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeChatBotAdminDevice", "device");
    YARP_REQUIRE_PLUGIN("chatBotAdmin_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking chatBotAdmin_nws_yarp device alone")
    {
        PolyDriver dd;

        ////////"Checking opening polydriver with no attached device"
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "chatBotAdmin_nws_yarp");
            pnws_cfg.put("name", "/chatBotAdmin_nws");
            REQUIRE(dd.open(pnws_cfg));
        }

        yarp::os::Time::delay(1.0);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    SECTION("Checking chatBotAdmin_nws_yarp device attached to fake device")
    {
        PolyDriver ddnws;
        PolyDriver ddfake;

        ////////"Checking opening polydriver and attach device"
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "chatBotAdmin_nws_yarp");
            pnws_cfg.put("name", "/chatBotAdmin_nws");
            REQUIRE(ddnws.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeChatBotAdminDevice");
            REQUIRE(ddfake.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws; ddnws.view(ww_nws);
            bool result_att = ww_nws->attach(&ddfake);
            REQUIRE(result_att); }
        }

        yarp::os::Time::delay(0.5);

        //"Close all polydrivers and check"
        {
            CHECK(ddnws.close());
            CHECK(ddfake.close());
        }
    }

    Network::setLocalMode(false);
}
