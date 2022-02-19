#include "serialPortWriteThread.h"
#include <iostream>
#include <utility>
namespace ly{
    /**
     * @brief 构造函数
     * @param portName 串口名称
     */
    serialPortWriteThread::serialPortWriteThread(int client)
    {
            clientID=client;
            simxGetObjectHandle(clientID, "yaw", &yawJoint, simx_opmode_blocking);
            simxGetObjectHandle(clientID, "pitch", &pitchJoint, simx_opmode_blocking);
            start(30000);
    }
    serialPortWriteThread::~serialPortWriteThread()
    {
    }
    /**
     * @brief 串口线程函数
     */
    void serialPortWriteThread::process()
    {
        // mutex_serial_send_.lock();
        writeData();
        // mutex_serial_send_.unlock();
        
    }
    /**
     * @brief 写数据
     */
    void serialPortWriteThread::writeData()
    {
	    unsigned char msg[max_receive_len_];

        mutex_serial_send_.lock();
        auto temp = send_;
        mutex_serial_send_.unlock();
        if(clientID!=-1) {
            if(yawJoint!=-1&&pitchJoint!=-1) {
//                std::cout<<temp.yaw/100.0<<" "<<temp.pitch/100.0<<std::endl;
                float yawTar,pitchTar;
                yawTar=temp.yaw/100.0/**3.1415926/180.0f*/;
                pitchTar=temp.pitch/100.0/**3.1415926/180.0f*/;
//                std::cout<<"send:"<<yawTar<<" "<<pitchTar<<std::endl;
                simxSetJointTargetPosition(clientID,yawJoint,yawTar,simx_opmode_oneshot);
                simxSetJointTargetPosition(clientID,pitchJoint,-pitchTar,simx_opmode_oneshot);
            }
        }
    }
    /**
     * @brief 设置需要发送的数据
     * @param data 发送数据
     */
    void serialPortWriteThread::setSendMsg(sendData data)
    {
        mutex_serial_send_.lock();
        send_ = data;
        mutex_serial_send_.unlock();
    }
}
