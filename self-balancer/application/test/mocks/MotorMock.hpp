#ifndef HAL_MOTOR_HPP
#define HAL_MOTOR_HPP

#include "Motor.hpp"
#include "gmock/gmock.h"

class MotorMock : public BaseMotor {
   public:
    MOCK_METHOD(void, setDutyCycle, (float dutyCycle), (override));
    MOCK_METHOD(float, getCurrent, (), (override));
    MOCK_METHOD(float, getDutyCycle, (), (override));
};

#endif  // HAL_MOTOR_HPP