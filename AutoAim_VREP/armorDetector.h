#ifndef __ARMOR_DETECTOR__
#define __ARMOR_DETECTOR__

#include "serialPortReadThread.h"
#include "serialPortWriteThread.h"
#include "cameraThread.h"
#include "Parameter.h"
#include "ProcessBox.h"


#include <boost/filesystem.hpp>
#include <fstream>

// #include <opencv2/video/tracking.hpp>
namespace ly
{
    class armorDetector
    {
    public:
        armorDetector();
        ~armorDetector();
        void Core();
        // void GetPose(std::vector<cv::Point2f>&,sendData&,cv::Mat&);

    private:
        cameraThread *camera_;
        serialPortReadThread *serialPortRead_;
        serialPortWriteThread *serialPortWrite_;
        int clientID=-1;
        int Port=3000;

        ProcessBox process_;
        std::vector<cv::Point2f> CORNERS;//corner at cam's frame       
    };
}



#endif //__ARMOR_DETECTOR__
