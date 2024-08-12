#ifndef IMU_MOCK_HPP
#define IMU_MOCK_HPP

#include "gmock/gmock.h"
#include "IMU.hpp"

class IMUMock : public BaseIMU {
    public:
    MOCK_METHOD0(getAcceleration, Vector3D());
    MOCK_METHOD0(getGyro, Vector3D());
};

#endif // IMU_MOCK_HPP