#include "app.h"
#include "HAL_LSM6DS3.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "wifi_cmds.h"
#ifdef __cplusplus
}
#endif

#define IMU_SPI_HOST SPI3_HOST
#define SPI1_PIN_NUM_MISO 13
#define SPI1_PIN_NUM_MOSI 11
#define SPI1_PIN_NUM_CLK  12
#define SPI1_PIN_NUM_CS   10

void app_run() {
    // create new LSM6DS3 object
    HAL_LSM6DS3 imu;

    imu.init(SPI3_HOST, SPI1_PIN_NUM_MISO, SPI1_PIN_NUM_MOSI, SPI1_PIN_NUM_CLK, SPI1_PIN_NUM_CS);

    while(true)
    {
        imu.poll();
        // send a udp packet with the message "hello world"
        char message[] = "hello world";
        send_udp_packet(message, sizeof(message));
        run_wifi_cmds();
        vTaskDelay(pdMS_TO_TICKS(100));  // Delay to print once per tick
    }

}