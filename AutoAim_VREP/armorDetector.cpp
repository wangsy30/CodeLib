#include "armorDetector.h"
#include<cmath>
#include<opencv2/core/eigen.hpp>

#define dt 0.03

using namespace cv;
using namespace std;

int cnt = 1;


namespace ly
{
    

    armorDetector::armorDetector()
    {
        clientID = simxStart("127.0.0.1", Port, true, true, 2000, 5);
        std::cout << clientID << std::endl;
#if defined(_CAM_)
        camera_ = new cameraThread(clientID);
#endif
#if defined(_READ_)
        serialPortRead_ = new serialPortReadThread(clientID);
#endif
#if defined(_WRITE_)
        serialPortWrite_ = new serialPortWriteThread(clientID);
#endif
    }
    armorDetector::~armorDetector()
    {
#if defined(_CAM_)
        delete camera_;
#endif
#if defined(_READ_)
        delete serialPortRead_;
#endif
#if defined(_WRITE_)
        delete serialPortWrite_;
#endif
        
    }

    void armorDetector::Core() {       
        cv::Mat src,src_roi;
        sendData send_data;//msg to send
        std::vector<receiveData> receive_data;
        receiveData Data;
        int start_num = 100;
        cv::waitKey(3000);
        startWindowThread();
        while (true) {
#if defined(_READ_)
            receive_data = serialPortRead_->getReceiveMsg();//get data from imu
            Data = receive_data.at(receive_data.size()-1);
            // cout << "[receive] yaw: " << Data.yaw << "   pitch: " << Data.pitch << endl;
#endif 

#if defined(_CAM_) && defined(_WRITE_)
            switch (waitKey(30)){
                case 'w':
                    send_data.pitch = send_data.pitch - 10;
                    break;
                case 's':
                    send_data.pitch = send_data.pitch + 10;
                    break;  
                case 'd':
                    send_data.yaw = send_data.yaw - 10;
                    break;
                case 'a':
                    send_data.yaw = send_data.yaw + 10;
                    break; 
                // case 'z':
                //     z = z + 0.5;
                //     break;
                // case 'x':
                //     z = z - 0.5;
                    break; 
                case 'l':
                    // cv::imwrite("./1.jpg",src);
                    return;
                default:
                    break;
            }
#endif 

#if defined(_CAM_)
            src = camera_->getFrame();
            if(src.empty()) {
                std::cout << "frame empty !" << std::endl;
                continue;
            }
            cv::line(src,cv::Point(src.cols/2-30,src.rows/2),cv::Point(src.cols/2+30,src.rows/2),cv::Scalar(0,0,255),2);
            cv::line(src,cv::Point(src.cols/2,src.rows/2-30),cv::Point(src.cols/2,src.rows/2+30),cv::Scalar(0,0,255),2);
            
            if(start_num > 0) {
                start_num--;
                imshow("dst",src);
                continue;
            }

            CORNERS.clear();
            if(!process_.FigureProcess(src,CORNERS,src_roi)){
                cout << "target lost !" << endl;
                imshow("dst",src);
                process_.SetFirstTime();//re-initialize the filter next time
                serialPortWrite_->setSendMsg(send_data);
                continue;
            }
            putText(src,format("DETECT NUMBER: %d",process_.DetectNumber(src_roi)),cv::Point(20, 100), cv::FONT_HERSHEY_PLAIN, 1.25, cv::Scalar(23, 243, 2), 2);
    #if defined(_WRITE_)
        #if defined(_READ_)
            process_.GetOrder(CORNERS,src,-Data.yaw,Data.pitch,send_data);
        #else
            process_.GetOrder(CORNERS,src,-send_data.yaw,-send_data.pitch,send_data);
        #endif
            putText(src,format("[SET]: pitch : %d  yaw : %d",-send_data.pitch,-send_data.yaw),cv::Point(20, 20), cv::FONT_HERSHEY_PLAIN, 1.25, cv::Scalar(23, 243, 2), 2);
            serialPortWrite_->setSendMsg(send_data);
    #endif
            imshow("dst",src);
    #if !defined(_WRITE_)
            if(waitKey(30) == 'l')
                return;
    #endif
#endif
        }
    }
}
