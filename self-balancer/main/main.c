
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lsm6ds3_reg.h"
#include "sdkconfig.h"
#include "time.h"
#include "wifi_cmds.h"
#include <string.h>
#include "nvs_flash.h"
#include "app.h"

#define IMU_SPI_HOST SPI3_HOST
#define SPI1_PIN_NUM_MISO 13
#define SPI1_PIN_NUM_MOSI 11
#define SPI1_PIN_NUM_CLK  12
#define SPI1_PIN_NUM_CS   10


void app_main(void) {
    printf("Hello world!\n");

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

    app_run();

    fflush(stdout);
    esp_restart();
}
