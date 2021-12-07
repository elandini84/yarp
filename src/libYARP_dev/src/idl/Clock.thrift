/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct ClockData
{
    1: i32 sec;
    2: i32 nsec;
} (
     yarp.api.include = "yarp/dev/api.h"
     yarp.api.keyword = "YARP_dev_API"
 )

service ClockRPC
{
    ClockData getClock();
} (
     yarp.api.include = "yarp/dev/api.h"
     yarp.api.keyword = "YARP_dev_API"
 )