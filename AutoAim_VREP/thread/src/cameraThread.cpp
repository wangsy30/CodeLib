#include "cameraThread.h"
namespace ly{
    /**
     * @brief 根据配置文件初始化不同的类
     * @param config 设备配置参数
     */
    cameraThread::cameraThread(int client)
    {
        clientID = client;
        start(30000);
    }

    /**
     * @brief 多线程获取相机信息
     */
    void cameraThread::process()
    {
        mutex_pic_.lock();
        frame_ = get_VREP();
        mutex_pic_.unlock();
    }
    /**
     * @brief 返回图片信息，相机和非相机两种情况
     * @return 图片信息
     */

    cv::Mat cameraThread::get_VREP()
    {
        simxInt resolution_buf[2] = {0};
        simxUChar *rgb_buff = NULL;
        // std::cout << clientID << std::endl;
//        clientID = simxStart("127.0.0.1", Port, true, true, 2000, 5);
        if (clientID != -1) {
            // printf("V-rep connected.");
                simxGetObjectHandle(clientID, "Vision_sensor", &camera_, simx_opmode_oneshot);
                if (camera_ != -1) {
                    simxGetVisionSensorImage(clientID, camera_, resolution_buf, &rgb_buff, 0, simx_opmode_streaming);

                    int buff_size = resolution_buf[0] * resolution_buf[1] * 3;
                    if (buff_size == 0) return cv::Mat();
                    simxUChar *RGB_buffer;
                    RGB_buffer = new simxUChar[buff_size];
                    memcpy(RGB_buffer, rgb_buff, buff_size * sizeof(simxUChar));
                    cv::Mat image_temp= cv::Mat::zeros(512, 1024, CV_8UC3);
                    if (rgb_buff != NULL && RGB_buffer != NULL) {
                        memcpy(image_temp.data, RGB_buffer, buff_size * sizeof(simxUChar));
                        free(RGB_buffer);
                        cv::flip(image_temp, image_temp, 0);
                        return image_temp;
                    }
                }
            else
                {
                    return cv::Mat();
                }
        }

        else
        {
            printf("V-rep disconnected.\n");
            return cv::Mat();
        }
    }

    cv::Mat cameraThread::getFrame()
    {
        cv::Mat temp1;
        mutex_pic_.lock();
        temp1 = frame_.clone();
        mutex_pic_.unlock();
        return temp1;
    }

}