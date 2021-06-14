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

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include "frameTransformGet_nws_ros.h"

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORGETNWSROS, "yarp.device.frameTransformGet_nws_ros")
}

//------------------------------------------------------------------------------------------------------------------------------
FrameTransformGet_nws_ros::FrameTransformGet_nws_ros(double tperiod) :
PeriodicThread(tperiod),
m_nodeName(ROSNODENAME),
m_topic(ROSTOPICNAME_TF),
m_rosNode(nullptr)
{
}

bool FrameTransformGet_nws_ros::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORGETNWSROS,"Error! YARP Network is not initialized");
        return false;
    }

    bool okGeneral = config.check("GENERAL");
    if(okGeneral)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        if (general_config.check("period"))
        {
            m_period = general_config.find("period").asDouble();
            setPeriod(m_period);
        }
    }

    //ROS configuration
    if (config.check("ROS"))
    {
        yCInfo(FRAMETRANSFORGETNWSROS, "Configuring ROS params");
        Bottle ROS_config = config.findGroup("ROS");
        if(ROS_config.check("ft_topic")) m_topic = ROS_config.find("ft_topic").asString();
        if(ROS_config.check("ft_node")) m_nodeName = ROS_config.find("ft_node").asString();

        //open ros publisher
        if (m_rosNode == nullptr)
        {
            m_rosNode = new yarp::os::Node(m_nodeName);
        }
        if (!m_rosPublisherPort_tf.topic(m_topic))
        {
            yCError(FRAMETRANSFORGETNWSROS) << "Unable to publish data on " << m_topic << " topic, check your yarp-ROS network configuration";
            return false;
        }
    }
    else
    {
        //no ROS options
        yCWarning(FRAMETRANSFORGETNWSROS) << "ROS Group not configured";
        return false;
    }

    start();

    return true;
}

bool FrameTransformGet_nws_ros::close()
{
    return true;
}

void FrameTransformGet_nws_ros::run()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    std::vector<yarp::math::FrameTransform> tempTfs;
    for (size_t i = 0; i < iGetIf.size(); i++)
    {
        tempTfs.clear();
        yCInfo(FRAMETRANSFORGETNWSROS, "Implement me");
        if(!iGetIf[i]->getTransforms(tempTfs))
        {
            yCError(FRAMETRANSFORGETNWSROS,"Unable to get the transform vector)");
        }
        publishFrameTransforms(tempTfs);
    }
}

bool FrameTransformGet_nws_ros::detachAll()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    iGetIf.clear();

    return true;
}

bool FrameTransformGet_nws_ros::attachAll(const yarp::dev::PolyDriverList& device2attach)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    pDriverList = device2attach;

    for (size_t i = 0; i < pDriverList.size(); i++)
    {
        yarp::dev::PolyDriver* pd = pDriverList[i]->poly;
        if (pd->isValid())
        {
            IFrameTransformStorageGet* pp=nullptr;
            if (pd->view(pp) && pp!=nullptr)
            {
                iGetIf.push_back(pp);
            }
        }
    }
    return true;
}

void  FrameTransformGet_nws_ros::yarpTransformToROSTransform(const yarp::math::FrameTransform &input, yarp::rosmsg::geometry_msgs::TransformStamped& output)
{
    output.child_frame_id = input.dst_frame_id;
    output.header.frame_id = input.src_frame_id;;
    output.header.stamp = input.timestamp;
    output.transform.rotation.x = input.rotation.x();
    output.transform.rotation.y = input.rotation.y();
    output.transform.rotation.z = input.rotation.z();
    output.transform.rotation.w = input.rotation.w();
    output.transform.translation.x = input.translation.tX;
    output.transform.translation.y = input.translation.tY;
    output.transform.translation.z = input.translation.tZ;
}

void FrameTransformGet_nws_ros::publishFrameTransforms(std::vector<yarp::math::FrameTransform>& transforms)
{
    static int rosMsgCounter = 0; // Ask for clarification!

    yarp::rosmsg::tf2_msgs::TFMessage& rosOutData = m_rosPublisherPort_tf.prepare();
    yarp::rosmsg::geometry_msgs::TransformStamped transform_timed;
    rosOutData.transforms.clear();

    for(auto& tf : transforms)
    {
        yarpTransformToROSTransform(tf,transform_timed);
        transform_timed.header.seq = rosMsgCounter;
        rosOutData.transforms.push_back(transform_timed);
    }
    m_rosPublisherPort_tf.write();

    rosMsgCounter++;
}
