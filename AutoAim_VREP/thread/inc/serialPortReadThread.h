#ifndef __SERIAL_READ_PORT_THREAD_H
#define __SERIAL_READ_PORT_THREAD_H

#include <cmath>
#include <mutex>
#include <thread>
#include "thread.h"
#include "../../CoppeliaSim/CoppeliaSim.h"
#include "../../CoppeliaSim/include/simConst.h"

namespace ly {
    struct receiveData {
        unsigned char flag;
        int yaw;
        short pitch;
    };

    // 串口设备的接收接口类
    class serialPortReadThread : public thread{
    public:
        serialPortReadThread(int client);

        ~serialPortReadThread();

        std::vector<receiveData> getReceiveMsg();

    private:
        void process();

        unsigned char temptemp[2 * 10];
        unsigned char temp[10];
        int max_receive_len_ = 10;
        unsigned char read_data_[10];
        receiveData stm32;

        void readData();

        struct timeval start_;
        int clientID = -1;
        int Port = 3000;
        int yawJoint = -1;
        int pitchJoint = -1;
    };

}
#endif //__SERIAL_PORT_THREAD_H
