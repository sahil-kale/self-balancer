#ifndef IMU_MOCK_HPP
#define IMU_MOCK_HPP

#include "IMU.hpp"
#include "gmock/gmock.h"

class IMUMock : public BaseIMU {
   public:
    MOCK_METHOD0(getAcceleration, Vector3D());
    MOCK_METHOD0(getGyro, Vector3D());
};

#endif  // IMU_MOCK_HPP