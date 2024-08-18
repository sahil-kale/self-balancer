#ifndef IMU_TELEM_HPP
#define IMU_TELEM_HPP
#include "IMU.hpp"
#include "MessageQueue.hpp"
#include "time.hpp"

class IMUTelem {
   public:
    IMUTelem(MessageQueue& messageQueue, BaseIMU& imu, TimeServer& timeServer);
    void run();

   private:
    MessageQueue& messageQueue_;
    BaseIMU& imu_;
    TimeServer& timeServer_;
};

#endif  // IMU_TELEM_HPP