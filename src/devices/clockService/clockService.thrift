/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yarp_dev_ClockData {
} (
  yarp.name = "yarp::dev::ClockData"
  yarp.includefile="yarp/dev/ClockData.h"
)

service ClockRPC
{
    yarp_dev_ClockData getClock();
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
