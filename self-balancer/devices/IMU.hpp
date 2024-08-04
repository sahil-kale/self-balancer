#ifndef IMU_HPP
#define IMU_HPP
#include <stdint.h>

class BaseIMU {
    public:
    typedef struct {
        float x;
        float y;
        float z;
        uint32_t timestamp;
        bool valid;
    } Vector3D;

    /**
     * @brief Get the acceleration of the IMU
     * @return Vector3D Acceleration in m/s^2
     */
    virtual Vector3D getAcceleration() = 0;

    /**
     * @brief Get the gyroscope of the IMU
     * @return Vector3D Gyroscope in rad/s
     */
    virtual Vector3D getGyro() = 0;
};

#endif // IMU_HPP