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
     * @return the clock as a yarp::dev::ClockData object
     */
    virtual yarp::dev::ClockData getClock() = 0;

    /**
     * Returns the current clock value
     * @param inputKey (std::string) The reader key
     * @param clockToFill (yarp::dev::ClockData) the clock as a yarp::dev::ClockData object
     * @return true if everything went fine
     */
    virtual bool getSynchroClock(std::string inputKey, yarp::dev::ClockData& clockToFill) = 0;

    /**
     * Sets a reader key
     * @param inputKey (std::string) The key to set
     * @return a boolean, true if the key has not already been used, false otherwise
     */
    virtual bool registerKey(std::string inputKey) = 0;
};

#endif // YARP_DEV_ICLOCK_H
