#ifndef _CONVERTER_H_
#define _CONVERTER_H_
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/LaserScan.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#include  "std_msgs/String.h"
#include "ProcessBox.h"
#include "define.h"
#include "Parameter.h"

class ImageConverter
{
    ros::NodeHandle nh_;

    ros::Publisher pub;
    ros::Subscriber sub;

    std_msgs::String send_msg;
     
   
    image_transport::ImageTransport it_;
    image_transport::Subscriber image_sub_;
    box::processbox pb;
    cv::Mat src;
    int flag;
    std::string OPENCV_WINDOW = "Image window";
    char picture_save_path[20] = {0};


    //lidar part
    std::vector<float> angle_ranges = {};
    float increment;
    float target_angle = 0;

    cv::VideoWriter writer;

public:
    ImageConverter();
    ~ImageConverter();
    void LaserScancallback(const sensor_msgs::LaserScan& scan);
    void DecodeAngle(std::vector<float>& input,std::vector<float>& output);
    void imageCb(const sensor_msgs::ImageConstPtr& msg);
};
#endif
