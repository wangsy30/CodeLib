#ifndef IRCAMERA_H
#define IRCAMERA_H
#include <iostream>
#include <unistd.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <cstdio>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>//for ying she
#include <opencv2/core/core.hpp>
#include <vector>
#include <pthread.h>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


class IRcamera{
public:
    IRcamera(){}
    ~IRcamera();

    int initCamera(int size_buffer = 0);
    void v4l2Init();
    void BuffToFrame();
    void ImDecode(std::vector<uchar>&);
    void DetectMask(cv::Mat&);
    void GetFrame(cv::Mat& dst);

    void RiseBias(){
        gray_bias += 10;
    }
    void DropBias(){
        gray_bias -= 10;
    }

    cv::VideoWriter writer;
private:
    int fd_video;
    unsigned int capture_width = 640;
    unsigned int capture_height = 512;
    unsigned int buf_count;
    cv::Mat frame;
    cv::Mat frame_tmp;
    unsigned char *mptr[2];//保存映射后用户空间的首地址
    unsigned int length[2];
    int ret;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int format = V4L2_PIX_FMT_YUYV;

    cv::Mat y;
    cv::Mat y1;
    cv::Mat y2;
    cv::Mat u;
    cv::Mat v;
    ushort gray_bias = 7990;

    cv::VideoCapture video;

    std::string read_path = "/home/w/mycode/opt22_code/ros_opencv/src/data_process/configs/test2.avi";
    std::string write_path = "/home/w/mycode/opt22_code/cam/configs/multi_res.avi";
};


#endif  // IRCAMERA_H
