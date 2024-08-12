#ifndef MOTOR_TELEM_HPP
#define MOTOR_TELEM_HPP
#include "MessageQueue.hpp"
#include "Motor.hpp"
#include "time.hpp"

class MotorTelem {
    public:
    MotorTelem(MessageQueue& messageQueue, BaseMotor& motor, TimeServer& timeServer);
    void run();

    private:
    MessageQueue& messageQueue_;
    BaseMotor& motor_;
    TimeServer& timeServer_;
};

#endif // IMU_TELEM_HPP