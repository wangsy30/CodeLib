#ifndef __SERIAL_WRITE_PORT_THREAD_H
#define __SERIAL_WRITE_PORT_THREAD_H

#include <mutex>
#include <thread>
#include "thread.h"
#include "../../CoppeliaSim/CoppeliaSim.h"
#include "../../CoppeliaSim/include/simConst.h"

namespace ly
{
    class sendData{
    public:
        unsigned char shootStatus;
        int yaw = 0;
        short pitch = 0;
        unsigned char distance;
    };
    // 串口设备的发送接口类
    class serialPortWriteThread:public thread
    {
    public:
        serialPortWriteThread(int client);
        ~serialPortWriteThread();
        void setSendMsg(sendData data);
    private:
        int max_receive_len_ = 10;
        sendData send_{};
        void process() override;
        void writeData();
        int clientID=-1;
        int yawJoint=-1;
        int pitchJoint=-1;
    };
}
#endif //__SERIAL_PORT_THREAD_H
