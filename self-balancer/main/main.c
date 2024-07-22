#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lsm6ds3_reg.h"
#include "sdkconfig.h"
#include "time.h"
#include "wifi_cmds.h"
#include <string.h>
#include "nvs_flash.h"

#define IMU_SPI_HOST SPI3_HOST
#define SPI1_PIN_NUM_MISO 13
#define SPI1_PIN_NUM_MOSI 11
#define SPI1_PIN_NUM_CLK  12
#define SPI1_PIN_NUM_CS   10
//#define SPI1_PIN_NUM_MISO 13
//#define SPI1_PIN_NUM_MOSI 11
//#define SPI1_PIN_NUM_CLK  12
//#define SPI1_PIN_NUM_CS   10

static spi_device_handle_t spi;

static const char *TAG = "LSM6DS3";

static spi_bus_config_t spi_bus_cfg = {
    .miso_io_num = SPI1_PIN_NUM_MISO,
    .mosi_io_num = SPI1_PIN_NUM_MOSI,
    .sclk_io_num = SPI1_PIN_NUM_CLK,
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
    .spics_io_num = SPI1_PIN_NUM_CS,
    .flags = 0,
    .queue_size = 1,
    .pre_cb = NULL,
    .post_cb = NULL
};

void init_spi() {
    esp_err_t ret;

    // Initialize the SPI bus
    ret = spi_bus_initialize(IMU_SPI_HOST, &spi_bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return;
    }

    // Add the device to the SPI bus
    ret = spi_bus_add_device(IMU_SPI_HOST, &spi_dev_cfg, &spi);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to add SPI device: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI("SPI", "SPI bus and device initialized successfully");
}

// Function to write to a register
int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len) {
    spi_device_handle_t *spi = (spi_device_handle_t *)handle;
    if (spi == NULL) {
        ESP_LOGE(TAG, "SPI handle is NULL");
        return -1;
    }

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));  // Zero out the transaction
    uint8_t data[len + 1];
    data[0] = reg;
    memcpy(&data[1], bufp, len);
    t.length = (len + 1) * 8;  // Length is in bits
    t.tx_buffer = data;
    t.user = (void *)0;

    esp_err_t ret = spi_device_transmit(*spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI Write Error: %s", esp_err_to_name(ret));
        return -1;
    }
    return 0;
}

// Function to read from a register
int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) {
    spi_device_handle_t *spi = (spi_device_handle_t *)handle;
    if (spi == NULL) {
        ESP_LOGE(TAG, "SPI handle is NULL");
        return -1;
    }

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

    esp_err_t ret = spi_device_transmit(*spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI Read Error: %s", esp_err_to_name(ret));
        return -1;
    }

    memcpy(bufp, &rx_data[1], len);
    return 0;
}

static stmdev_ctx_t imu_ctx = {
    .write_reg = platform_write,
    .read_reg = platform_read,
    .handle = &spi
};

void app_main(void) {
    printf("Hello world!\n");

    init_spi();

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    //Initalize wifi
    if(wifi_init_sta())
    {
        ESP_LOGE("main", "Failed to initialize wifi");
    }
    else
    {
        ESP_LOGI("main", "Wifi initialized");
    }
    vTaskDelay(pdMS_TO_TICKS(1));

    lsm6ds3_reset_set(&imu_ctx, 1);
    // wait 100ms, use a freertos delay
    vTaskDelay(pdMS_TO_TICKS(100));

    // disable I2C
    lsm6ds3_i2c_interface_set(&imu_ctx, 1);

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

    // get the FS value for gyro 
    const float gyro_sensitivity = 8.75; // mdps/LSB
    const float gyro_scale = gyro_sensitivity * (M_PI / 180.0f) * 0.001f;  // Convert mdps to rad/s

    // get the FS value for accelerometer
    const float accel_sensitivity = 0.061; // mg/LSB
    const float accel_scale = accel_sensitivity * 9.81f / 1000.0f;  // Convert mg to m/s²

    while (whoami == LSM6DS3_ID) {
        lsm6ds3_status_reg_t status;
        lsm6ds3_status_reg_get(&imu_ctx, &status);

        if (status.xlda) {  // Accelerometer data available
            int16_t accel_data_raw[3];
            lsm6ds3_acceleration_raw_get(&imu_ctx, accel_data_raw);
            float accel_data[3];
            for (int i = 0; i < 3; i++) {
                accel_data[i] = accel_data_raw[i] * accel_scale;  // Convert mg to m/s²
            }
#ifdef PRINT_DATA
            printf("Timestamp: %lu, Acceleration: x=%.2f m/s², y=%.2f m/s², z=%.2f m/s²\n", 
                    (unsigned long)time(NULL), accel_data[0], accel_data[1], accel_data[2]);
#endif
        }

        if (status.gda) {  // Gyroscope data available
            int16_t gyro_data_raw[3];
            lsm6ds3_angular_rate_raw_get(&imu_ctx, gyro_data_raw);
            float gyro_data[3];
            for (int i = 0; i < 3; i++) {
                gyro_data[i] = gyro_data_raw[i] * gyro_scale;  // Convert mdps to rad/s
            }
#ifdef PRINT_DATA
            printf("Timestamp: %lu, Gyroscope: x=%.2f rad/s, y=%.2f rad/s, z=%.2f rad/s\n", 
                    (unsigned long)time(NULL), gyro_data[0], gyro_data[1], gyro_data[2]);
#endif
        }

        if (status.tda) {  // Temperature data available
            int16_t temperature_raw;
            lsm6ds3_temperature_raw_get(&imu_ctx, &temperature_raw);
            float temperature = temperature_raw / 16.0f + 25.0f;  // Convert to °C
#ifdef PRINT_DATA
            printf("Timestamp: %lu, Temperature: %.2f °C\n", 
                    (unsigned long)time(NULL), temperature);
#endif
        }

        // send a udp packet with the message "hello world"
        char message[] = "hello world";
        send_udp_packet(message, sizeof(message));
        run_wifi_cmds();
        vTaskDelay(pdMS_TO_TICKS(1));  // Delay to print once per tick
    }

    fflush(stdout);
    esp_restart();
}
