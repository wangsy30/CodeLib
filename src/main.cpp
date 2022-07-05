#include "Converter.h"


int main(int argc, char** argv)
{
    std::string para_path = "/home/w/mycode/opt22_code/ros_opencv/src/data_process/configs/parameters.xml";
    std::string mask_test_path = "../test/";
    para::Parameter parameter(para_path);
    parameter.load();
    ros::init(argc, argv, "ProcessCenter");
    ImageConverter ic;
    ros::spin();
    return 0;
}
