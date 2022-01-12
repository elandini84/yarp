/*
* SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
* SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef YARP_CLOCK_NWS_YARP_H
#define YARP_CLOCK_NWS_YARP_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IClock.h>
#include <yarp/os/PeriodicThread.h>
#include <ClockRPC.h>

#define CLOCK_PERIOD_DEF 0.02

/**
 * @ingroup dev_impl_nws_yarp
 *
 * \section clock_nws_yarp_parameters Device description
 * @brief `clock_nws_yarp`: A network wrapper client which converts the input retrieved from an IClock interface to a ClockRPC thrift interface.
 * This device listens on a ClockRPC interface for an RPC and then forwards the request to an IClock interface (device attached)
 * This device is paired with its client called Clock_nwc_yarp.
 * A part from the rpc calls, the Clock_nws_yarp continuously asks the clock value to the attached device and publishes it on a yarp port (if enabled)
 *
 * \section Clock_nws_yarp_device_parameters Parameters
 *   Parameters required by this device are:
 * | Parameter name               | SubParameter  | Type    | Units  | Default Value | Required  | Description                                                | Notes                      |
 * |:----------------------------:|:-------------:|:-------:|:------:|:-------------:|:---------:|:----------------------------------------------------------:|:--------------------------:|
 * | period                       |      -        | double  | s      | 0.02          | No        | Periodic thread period in seconds                          | default 0.02s              |
 * | nws_thrift_port_prefix       |      -        | string  | -      | ""            | No        | a prefix for the nws thrift rpc port name                  |                            |
 * | output_streaming_port_prefix |      -        | string  | -      | ""            | No        | a prefix for the output streaming port name                |                            |
 * | streaming_enabled            |      -        | int     | -      | 1             | No        | enable/disable the clock publishing on the streaming port  | 0 = disabled; 1 = enabled  |
 *
 * If the two port prefixes are not specified, the port names will be as follows
 * -# thrift port = /clock_nws_yarp/thrift
 * -# output streaming port = /clock
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device                        clock_nws_yarp
 * nws_thrift_port_prefix        /test_server
 * output_streaming_port_prefix  /test_stream
 * period                        0.02
 * \endcode
 *
 * example of xml file
 *
 * \code{.unparsed}
 * <?xml version="1.0" encoding="UTF-8" ?>
 * <!DOCTYPE robot PUBLIC "-//YARP//DTD yarprobotinterface 3.0//EN" "http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd">
 * <robot name="fakeClock" build="2" portprefix="clockNwsYarp" xmlns:xi="http://www.w3.org/2001/XInclude">
 *   <devices>
 *     <device xmlns:xi="http://www.w3.org/2001/XInclude" name="fakeClock_device" type="fakeClock">
 *     </device>
 *     <device xmlns:xi="http://www.w3.org/2001/XInclude" name="clockNwsYarp_device" type="clock_nws_yarp">
 *       <action phase="startup" level="5" type="attach">
 *         <paramlist name="networks">
 *           <elem name="subdevicegetyarp"> fakeClock_device </elem>
 *         </paramlist>
 *       </action>
 *     </device>
 *   </devices>
 * </robot>
 * \endcode
 *
 */

class Clock_nws_yarp : public yarp::dev::DeviceDriver,
                       public yarp::dev::WrapperSingle,
                       //public yarp::os::PeriodicThread,
                       yarp::os::Thread,
                       public ClockRPC
{
public:
    // Constructors and destructor
    Clock_nws_yarp();
    Clock_nws_yarp(const Clock_nws_yarp&) = delete;
    Clock_nws_yarp(Clock_nws_yarp&&) = delete;
    Clock_nws_yarp& operator=(const Clock_nws_yarp&) = delete;
    Clock_nws_yarp& operator=(Clock_nws_yarp&&) = delete;
    ~Clock_nws_yarp() = default;

    // ClockRPC
    return_getClock getClock() override;

    // yarp::dev::DeviceDriver
    bool  open(yarp::os::Searchable &params) override;
    bool  close() override;

    // yarp::dev::WrapperSingle
    bool  attach(yarp::dev::PolyDriver* deviceToAttach) override;
    bool  detach() override;

    // yarp::os::PeriodicThread
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    bool                m_streaming_port_enabled{true};
    double              m_period{CLOCK_PERIOD_DEF};
    std::string         m_serverThriftPortName{"/clock_nws_yarp/thrift"};
    std::string         m_outputStreamPortName{"/clock"};
    yarp::os::Port      m_serverThriftPort;
    yarp::os::Port      m_outputStreamPort;
    yarp::dev::IClock  *m_IClock{nullptr};
};

#endif // YARP_CLOCK_NWS_YARP_H
