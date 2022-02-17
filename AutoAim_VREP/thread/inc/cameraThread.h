#ifndef __CAMERA_THREAD_H
#define __CAMERA_THREAD_H

#include <mutex>
#include <thread>
#include "thread.h"
#include <cstring>
#include <opencv2/opencv.hpp>
#include "CoppeliaSim.h"
#include "../../CoppeliaSim/include/simConst.h"


namespace ly{

    class cameraThread: public thread
    {
    public:
        cameraThread(int client);
        cv::Mat get_VREP();
        cv::Mat getFrame();


    private:
        void process();
        cv::Mat frame_;
        int clientID = -1;
        int camera_ = -1;
    };
}


#endif //__CAMERA_THREAD_H