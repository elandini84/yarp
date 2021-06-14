/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef YARP_DEV_FRAMETRANSFORMGETNWSROS_H
#define YARP_DEV_FRAMETRANSFORMGETNWSROS_H


#include <yarp/os/Network.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <mutex>
#include <map>

#include <yarp/math/FrameTransform.h>

#include <yarp/rosmsg/geometry_msgs/TransformStamped.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>

#define ROSNODENAME "/tfNode"
#define ROSTOPICNAME_TF "/tf"

class FrameTransformGet_nws_ros :
    public yarp::dev::DeviceDriver,
    public yarp::os::PeriodicThread,
    public yarp::dev::WrapperSingle
{
private:
    std::vector<yarp::math::FrameTransform> m_transforms;
    mutable std::mutex                      m_trf_mutex;

public:
    FrameTransformGet_nws_ros(double tperiod=0.010);
    ~FrameTransformGet_nws_ros() {}

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //wrapper and interfaces
    bool attach(yarp::dev::PolyDriver* device2attach) override;
    bool detach() override;

    //periodicThread
    void     run() override;

    //own
    void publishFrameTransforms(std::vector<yarp::math::FrameTransform>& transforms);
    void yarpTransformToROSTransform(const yarp::math::FrameTransform &input, yarp::rosmsg::geometry_msgs::TransformStamped& output);

private:
    mutable std::mutex                                     m_pd_mutex;
    std::string                                            m_nodeName;
    std::string                                            m_topic;
    double                                                 m_period;
    yarp::dev::PolyDriver*                                 m_pDriver;
    yarp::dev::IFrameTransformStorageGet*                  m_iGetIf;
    yarp::os::Node*                                        m_rosNode;
    yarp::os::Publisher<yarp::rosmsg::tf2_msgs::TFMessage> m_rosPublisherPort_tf;
};

#endif // YARP_DEV_FRAMETRANSFORMGETNWSROS_H
