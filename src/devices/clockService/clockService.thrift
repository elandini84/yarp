/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yarp_dev_ClockData {
} (
  yarp.name = "yarp::dev::ClockData"
  yarp.includefile="yarp/dev/ClockData.h"
)

struct return_getClock
{
    1: bool retValue;
    2: yarp_dev_ClockData returnedClock;
}

service ClockRPC
{
    return_getClock getClock();
}
