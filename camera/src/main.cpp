#include "IRCamera.h"
#include "define.h"

#include <ros/ros.h>
#include <stdio.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

// void ImgPublish(cv::Mat& src){

// }

int main(int argc, char* argv[])
{
    ros::init(argc, argv, "IRCamera");
    ros::NodeHandle node;
    image_transport::ImageTransport transport(node);
    image_transport::Publisher image_pub; 
    image_pub=transport.advertise("IRImage_raw", 10);
    ros::Rate loop_rate(50);

    std::string mask_test_path = "/home/w/mycode/opt22_code/cam/test/";
#if defined(_PICTURE_)
    cv::Mat src,dst;
    int count = stoi(argv[1]);
    char picture_path[50] = {0};
    sprintf(picture_path,"%06d.jpg",count);
    src = imread(mask_test_path+picture_path);
    cv::waitKey(0);
#endif

#if defined(_VIDEO_)
    IRcamera camera;
    camera.initCamera(0);

    cv::Mat src;
    char shut_key = 1;
    int count = 0;
    char save_path[50] = {0};
    int flag;
    while(ros::ok() && shut_key){
        camera.GetFrame(src);
        if(src.empty()){
            cout << "frame empty" << endl;
            break;
        }
        ros::Time time=ros::Time::now();
        cv_bridge::CvImage cvi;
        cvi.header.stamp = time;
        cvi.header.frame_id = "image";
        cvi.encoding = "bgr8";
        cvi.image = src;

        sensor_msgs::Image im;
        cvi.toImageMsg(im);
        image_pub.publish(im);
        ROS_INFO("Converted Successfully!");

        ros::spinOnce();
        loop_rate.sleep();
    }
#endif
    return 0;
}

