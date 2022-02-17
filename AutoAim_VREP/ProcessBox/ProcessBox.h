#ifndef _PROCESS_BOX_
#define _PROCESS_BOX_

#include <opencv2/opencv.hpp>

#include "../thread/inc/serialPortWriteThread.h"
#include "../para/Parameter.h"
#include "../control.h"
class ProcessBox
{
private:
    cv::Ptr<cv::ml::SVM> SVM_params;//SVM Machine
    cv::Mat rvec,tvec;
    Eigen::Matrix<double,3,1> Tvec_cam,Tvec_base;
    Eigen::Matrix3d intrMatrix_eigen;//inter para matrix
    double theta_y,theta_x;//current pose of camera
    Eigen::Matrix3d RotMatrix,R_y,R_x;//camera to base
    float yaw_m[2],pitch_m[2];//measurement of the center's position
    float v_yaw_m,v_pitch_m;//measurement of the ceter's velocity

    cv::KalmanFilter kalman;
    int dt = 0.03;
    unsigned char first_time = 1;
    cv::Mat measurement;
    cv::Mat pose_correct;
public:
    ProcessBox(/* args */);
    ~ProcessBox();
    bool FigureProcess(cv::Mat& src,std::vector<cv::Point2f>& corners,cv::Mat& src_roi);
    int DetectNumber(cv::Mat& src_roi);
    void GetOrder(const std::vector<cv::Point2f>& corners,cv::Mat& src,const int& yaw,const int& pitch,ly::sendData& send_data);
    void CalTargetPose(const std::vector<cv::Point2f>& corners,const int& yaw,const int& pitch);
    void feature(cv::Mat image,cv::Mat& data_mat,const int& yangben_data_position);
    void KalmanPredict();
    inline void SetFirstTime() {
        first_time = 1;
    }
};
#endif