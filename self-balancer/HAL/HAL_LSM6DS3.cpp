#include "HAL_LSM6DS3.hpp"
#include "esp_log.h"
#include "string.h"
#include "time.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

HAL_LSM6DS3::HAL_LSM6DS3() {
    imu_ctx.write_reg = [](void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len) -> int32_t {
        return static_cast<HAL_LSM6DS3*>(handle)->platform_write(reg, bufp, len);
    };

    imu_ctx.read_reg = [](void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) -> int32_t {
        return static_cast<HAL_LSM6DS3*>(handle)->platform_read(reg, bufp, len);
    };

    imu_ctx.handle = this;
}

HAL_LSM6DS3::~HAL_LSM6DS3() {
    // Deinitialize the IMU
}

void HAL_LSM6DS3::init(spi_host_device_t spi_host, uint8_t miso, uint8_t mosi, uint8_t clk, uint8_t cs) {
    static spi_bus_config_t spi_bus_cfg = {
        .mosi_io_num = mosi,
        .miso_io_num = miso,
        .sclk_io_num = clk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };

    static spi_device_interface_config_t spi_dev_cfg = {
        .command_bits = 8,
        .address_bits = 8,
        .dummy_bits = 0,
        .mode = 3,
        .duty_cycle_pos = 128,  // 50% duty cycle
        .cs_ena_pretrans = 1,
        .cs_ena_posttrans = 1,
        .clock_speed_hz = 1000000,  // 1 MHz
        .input_delay_ns = 0,
        .spics_io_num = cs,
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL
    };

    esp_err_t ret;

    // Initialize the SPI bus
    ret = spi_bus_initialize(spi_host, &spi_bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(this->TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return;
    }

    // Add the device to the SPI bus
    ret = spi_bus_add_device(spi_host, &spi_dev_cfg, &spi);
    if (ret != ESP_OK) {
        ESP_LOGE(this->TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(this->TAG, "SPI bus and device initialized successfully");

    lsm6ds3_reset_set(&imu_ctx, 1);

    // disable I2C
    lsm6ds3_i2c_interface_set(&imu_ctx, LSM6DS3_I2C_DISABLE);

    // set XL ODR
    lsm6ds3_xl_data_rate_set(&imu_ctx, LSM6DS3_XL_ODR_1k66Hz);

    // set Gyro ODR
    lsm6ds3_gy_data_rate_set(&imu_ctx, LSM6DS3_GY_ODR_1k66Hz);

    uint8_t whoami = 0x00;
    int32_t wifiret = lsm6ds3_device_id_get(&imu_ctx, &whoami);
    if (wifiret == 0) {
        printf("WHO_AM_I register: 0x%02x\n", whoami);
    } else {
        printf("Failed to read WHO_AM_I register\n");
    }

    const float gyro_sensitivity = 8.75; // mdps/LSB
    this->gyro_scale = gyro_sensitivity * (M_PI / 180.0f) * 0.001f;  // Convert mdps to rad/s

    // get the FS value for accelerometer
    const float accel_sensitivity = 0.061; // mg/LSB
    this->accel_scale = accel_sensitivity * 9.81f / 1000.0f;  // Convert mg to m/s²

}

void HAL_LSM6DS3::poll() {
    lsm6ds3_status_reg_t status;
    lsm6ds3_status_reg_get(&imu_ctx, &status);

    if (status.xlda) {  // Accelerometer data available
        int16_t accel_data_raw[3];
        lsm6ds3_acceleration_raw_get(&imu_ctx, accel_data_raw);
        float accel_data[3];
        for (int i = 0; i < 3; i++) {
            accel_data[i] = accel_data_raw[i] * accel_scale;  // Convert mg to m/s²
        }
        
        // store in acceleration vector
        this->acceleration.x = accel_data[0];
        this->acceleration.y = accel_data[1];
        this->acceleration.z = accel_data[2];
        this->acceleration.timestamp = (uint32_t)time(NULL);
        this->acceleration.valid = true;

        printf("Timestamp: %lu, Acceleration: x=%.2f m/s², y=%.2f m/s², z=%.2f m/s²\n", 
                (unsigned long)time(NULL), accel_data[0], accel_data[1], accel_data[2]);
    }

    if (status.gda) {  // Gyroscope data available
        int16_t gyro_data_raw[3];
        lsm6ds3_angular_rate_raw_get(&imu_ctx, gyro_data_raw);
        float gyro_data[3];
        for (int i = 0; i < 3; i++) {
            gyro_data[i] = gyro_data_raw[i] * gyro_scale;  // Convert mdps to rad/s
        }

        // store in gyro vector
        this->gyro.x = gyro_data[0];
        this->gyro.y = gyro_data[1];
        this->gyro.z = gyro_data[2];
        this->gyro.timestamp = (uint32_t)time(NULL);
        this->gyro.valid = true;
        printf("Timestamp: %lu, Gyroscope: x=%.2f rad/s, y=%.2f rad/s, z=%.2f rad/s\n", 
                (unsigned long)time(NULL), gyro_data[0], gyro_data[1], gyro_data[2]);
    }

    if (status.tda) {  // Temperature data available
        int16_t temperature_raw;
        lsm6ds3_temperature_raw_get(&imu_ctx, &temperature_raw);
        this->temperature = temperature_raw / 16.0f + 25.0f;  // Convert to °C
        printf("Timestamp: %lu, Temperature: %.2f °C\n", 
                (unsigned long)time(NULL), this->temperature);
    }
}

// Function to write to a register
int32_t HAL_LSM6DS3::platform_write(uint8_t reg, const uint8_t *bufp, uint16_t len) {

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));  // Zero out the transaction
    uint8_t data[len + 1];
    data[0] = reg;
    memcpy(&data[1], bufp, len);
    t.length = (len + 1) * 8;  // Length is in bits
    t.tx_buffer = data;
    t.user = (void *)0;

    esp_err_t ret = spi_device_transmit(spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI Write Error: %s", esp_err_to_name(ret));
        return -1;
    }
    return 0;
}

BaseIMU::Vector3D HAL_LSM6DS3::getAcceleration() {
    return this->acceleration;
}

BaseIMU::Vector3D HAL_LSM6DS3::getGyro() {
    return this->gyro;
}

// Function to read from a register
int32_t HAL_LSM6DS3::platform_read(uint8_t reg, uint8_t *bufp, uint16_t len) {
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));  // Zero out the transaction
    uint8_t tx_data[len + 1];
    // zero out tx data
    memset(tx_data, 0, len + 1);
    uint8_t rx_data[len + 1];
    memset(rx_data, 0, len + 1);

    tx_data[0] = reg | 0x80;  // Set the read bit
    t.length = (len + 1) * 8;  // Length is in bits
    t.tx_buffer = tx_data;
    t.rxlength = len * 8;
    t.rx_buffer = rx_data;
    t.user = (void *)0;

    esp_err_t ret = spi_device_transmit(spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI Read Error: %s", esp_err_to_name(ret));
        return -1;
    }

    memcpy(bufp, &rx_data[1], len);
    return 0;
}

#pragma GCC diagnostic pop
