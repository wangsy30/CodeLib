#ifndef __PROCESSBOX_H
#define __PROCESSBOX_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <bits/stdc++.h>
#include <fstream>


#include "../yolo/class_timer.hpp"
#include "../yolo/class_detector.h"
#define opt_pi 3.1415926
namespace box {


enum JudgeState{
    FEVER_AND_MASKLESS=100,
    FEVER_ONLY,
    MASKLESS_ONLY,
    NEITHER,
    JUDGE_FAILED
};

enum MODE{
    BOTH_MODE=200,
    FEVER_MODE,
    MASKLESS_MODE,
    NONE_MODE
};

class processbox{
public:
    processbox();
    ~processbox() {}

//    bool DetectMask(cv::Mat&);
    int Process(cv::Mat&,std::vector<float>&,float);
    int DetectFace(std::vector<float>& angle_ranges);
    int JudgeFace(cv::Mat&,float face_range,std::vector<int> face_location);
    int MaskLessFound(cv::Mat&);
    bool FeverFound(cv::Mat&);

    /*--lidar part--*/
    float Index2Angle(int index,int length);
    float Pixel2Range(int pixel,std::vector<float>& angle_ranges);
private:
    float temperature = 0.0;

    cv::Mat _src;
    cv::Mat mask;
    cv::Mat dst_mask;

    std::vector<cv::Mat> face_roi_list;

    Config config;

    Timer timer;
    Detector detector;
    cv::Mat src_bgr;

    std::vector<int> judgeflag;
    int judge_tmp;
    int fever_num;
    int maskless_num;
    double minval = 0.0,fever_val = 0.0;

    cv::Mat src_uphalf;
    cv::Mat head;
    cv::Rect halfhead;
    cv::Rect finalhead;
    cv::Mat halfhead_mat;

    cv::Mat src_gray;

//    std::vector<float> angle_ranges;
    std::vector<float> face_ranges;
    std::vector<std::vector<int>> face_locations;
    float _increment;
};

}

#endif
