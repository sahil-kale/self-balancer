#ifndef MOTOR_TELEM_HPP
#define MOTOR_TELEM_HPP
#include "MessageQueue.hpp"
#include "Motor.hpp"
#include "time.hpp"
#include "messages/header/header.pb.h"

class MotorTelem {
    public:
    MotorTelem(MessageQueue& messageQueue, BaseMotor& motor, TimeServer& timeServer, MessageChannels channel);
    void run();

    private:
    MessageQueue& messageQueue_;
    BaseMotor& motor_;
    TimeServer& timeServer_;
    MessageChannels channel_;
};

#endif // IMU_TELEM_HPP