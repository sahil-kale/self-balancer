#ifndef HAL_LSM6DS3_HPP
#define HAL_LSM6DS3_HPP

#include "IMU.hpp"
#include "driver/spi_master.h"
#include "lsm6ds3_reg.h"

class HAL_LSM6DS3 : public BaseIMU {
   public:
    HAL_LSM6DS3();
    virtual ~HAL_LSM6DS3();

    /**
     * @brief Initialize the IMU with the ESP32 SPI bus
     * @param spi_host The SPI host to use
     * @param miso The MISO pin
     * @param mosi The MOSI pin
     * @param clk The clock pin
     * @param cs The chip select pin
     */
    void init(spi_host_device_t spi_host, uint8_t miso, uint8_t mosi, uint8_t clk, uint8_t cs);
    void poll();  // Call at 1ms

    Vector3D getAcceleration() override;
    Vector3D getGyro() override;

    float getTemperature();

   private:
    bool imuInitialized = false;
    Vector3D acceleration;
    Vector3D gyro;
    float temperature;

    float gyro_scale = 0.0f;
    float accel_scale = 0.0f;

    spi_device_handle_t spi;
    stmdev_ctx_t imu_ctx;
    const char *TAG = "LSM6DS3";

    int32_t platform_read(uint8_t reg, uint8_t *bufp, uint16_t len);
    int32_t platform_write(uint8_t reg, const uint8_t *bufp, uint16_t len);
};

#endif  // HAL_LSM6DS3_HPP