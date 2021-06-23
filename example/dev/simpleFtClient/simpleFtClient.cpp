#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <yarp/sig/Matrix.h>
#include <thread>

int main(int argc, char* argv[])
{
    yarp::os::Network n;

    yarp::os::ResourceFinder rf;
    rf.configure(argc,argv);

    yarp::dev::IFrameTransform* ft;
    yarp::dev::PolyDriver pd;
    yarp::os::Property p;
    p.put("device","frameTransformClient");
    if(rf.check("filexml_option")) 
    {
        p.put("filexml_option",rf.find("filexml_option").asString());
    }
    else
    {
        p.put("filexml_option","/mnt/c/ExchangeData/robotinterface/FrameTransform/ftc_yarp_only/ftc_pub_yarp_only.xml");
    }
    pd.open(p);
    printf("Line: %d\n",__LINE__);
    pd.view(ft);
    printf("Line: %d\n",__LINE__);

    // Fake transform
    std::string src = "/arm";
    std::string dest = "/hand";
    yarp::sig::Matrix armToHand(4,4);
    yarp::sig::Matrix result(4,4);
    armToHand.zero();
    for(int i=0; i<4; i++)
    {
        armToHand(i,i) = 1;
    }

    while(true)
    {
        printf("Set: %d\n",ft->setTransform(dest,src,armToHand));
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        printf("Get: %d\n",ft->getTransform(dest,src,result));
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        printf("El 0,0: %f\n",result(0,0));
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        printf("Clear: %d\n",ft->clear());
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        printf("Get: %d\n",ft->getTransform(dest,src,result));
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    return 0;
}