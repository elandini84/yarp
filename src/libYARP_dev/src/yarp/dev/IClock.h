/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICLOCK_H
#define YARP_DEV_ICLOCK_H

#include <yarp/dev/ClockData.h>
#include <string>

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>

/*! \file IClock.h clock interface */
namespace yarp::dev {
class IClock;
}

/**
 * @ingroup dev_iface_other
 *
 * A generic clock providing interface
 */
class YARP_dev_API yarp::dev::IClock
{
public:

    virtual ~IClock();

    /**
     * Returns the current clock value
     * @return the clock in seconds
     */
    virtual ClockData getClock() = 0;

};

#endif // YARP_DEV_ICLOCK_H
