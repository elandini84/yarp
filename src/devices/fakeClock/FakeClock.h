/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKECLOCK_H
#define YARP_FAKECLOCK_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IClock.h>

constexpr double default_period = 0.02;

/**
 * @ingroup dev_impl_fake dev_impl_others
 *
 * \section fakeClock_parameters Device description
 * \brief `FakeClock`: A device for generating a fake odometry.
 * This device will update and internal `clock` value based on the `yarp::os::Time::now()` output.
 *
 *   Parameters required by this device are:
 * | Parameter name | SubParameter  | Type    | Units  | Default Value | Required   | Description                                            | Notes         |
 * |:--------------:|:-------------:|:-------:|:------:|:-------------:|:---------: |:------------------------------------------------------:|:-------------:|
 * | period         |      -        | double  | s      |   0.02        | No         | refresh period of the internally stored clock value    | default 0.02s |
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device FakeClock
 * period 0.02
 * \endcode
 *
 * example of xml file
 *
 * \code{.unparsed}
 * <?xml version="1.0" encoding="UTF-8" ?>
 * <!DOCTYPE robot PUBLIC "-//YARP//DTD yarprobotinterface 3.0//EN" "http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd">
 * <robot name="fakeClock" build="2" portprefix="fakeClock" xmlns:xi="http://www.w3.org/2001/XInclude">
 *   <devices>
 *     <device xmlns:xi="http://www.w3.org/2001/XInclude" name="fakeClock_device" type="fakeClock">
 *     </device>
 *   </devices>
 * </robot>
 * \endcode
 *
 * example of command via terminal.
 *
 * \code{.unparsed}
 * yarpdev --device fakeClock
 * \endcode
 */

class FakeClock :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IClock
{
public:
    FakeClock();
    ~FakeClock() override;

    yarp::dev::ClockData convertTime(double input);

    // PeriodicThread
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // IClock
    yarp::dev::ClockData getClock() override;

private:
    yarp::dev::ClockData m_clockData;
    std::mutex m_clock_mutex;
    double m_period;
};

#endif // YARP_FAKECLOCK_H
