#include "serialPortReadThread.h"
#include <iostream>
#include <utility>
namespace ly {
    std::vector<receiveData> receive_;

    /**
     * @brief 构造函数
     * @param portName 串口名称
     */
    serialPortReadThread::serialPortReadThread(int client) {
        clientID = client;
        simxGetObjectHandle(clientID, "yaw", &yawJoint, simx_opmode_blocking);
        simxGetObjectHandle(clientID, "pitch", &pitchJoint, simx_opmode_blocking);
        receive_.resize(150);
         start(1000);
    }

    serialPortReadThread::~serialPortReadThread() {
        //delete read_data_;
    }

    /**
     * @brief 串口线程函数
     */
    void serialPortReadThread::process() {
        // mutex_serial_receive_.lock();
        readData();
        // mutex_serial_receive_.unlock();
    }

    /**
     * @brief 读数据
     */
    void serialPortReadThread::readData() {
        short flag, counter, Gyro_Pitch, Gyro_Yaw;
        simxFloat vrepYaw, vrepPitch;
        if (clientID != -1) {
            if (yawJoint != -1 && pitchJoint != -1) {
                simxGetJointPosition(clientID, yawJoint, &vrepYaw, simx_opmode_oneshot);
                simxGetJointPosition(clientID, pitchJoint, &vrepPitch, simx_opmode_oneshot);
                vrepYaw *= 100;
                vrepPitch *= 100;
                stm32.flag = 0;
                stm32.pitch = std::round(vrepPitch);
                stm32.yaw = std::round(vrepYaw);
                //std::cout<<vrepYaw<<" "<<stm32.pitch<<std::endl;
            }
        }
        //互斥锁
        mutex_serial_receive_.lock();
        if (receive_.size() > 149) {
            receive_.erase(receive_.begin());
        }
        receive_.emplace_back(stm32);

        mutex_serial_receive_.unlock();
    }

    /**
     * @brief 获取收到的数据
     * @return 当前数据
     */
    std::vector<receiveData> serialPortReadThread::getReceiveMsg() {
        mutex_serial_receive_.lock();
        auto data = receive_;
        mutex_serial_receive_.unlock();
        return data;
    }
}
