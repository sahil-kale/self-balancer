#include "app.h"
#include "HAL_LSM6DS3.hpp"
#include "HAL_Motor.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "time.h"
#include "driver/mcpwm_prelude.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

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

#define TIMER_FREQ_HZ 1000000 // 1MHz timer frequency
#define TIMER_PERIOD_US 100 // 10kHz PWM frequency

#define PWML_GPIO 21
#define PWMR_GPIO 26
#define DIRL_GPIO 33
#define DIRR_GPIO 34
#define BRAKE_GPIO 35

#define CURR_SENSE_L_GPIO 1
#define CURR_SENSE_L_ADC_CHANNEL ADC_CHANNEL_0
#define CURR_SENSE_R_GPIO 2
#define CURR_SENSE_R_ADC_CHANNEL ADC_CHANNEL_1
#define MOTOR_SLEEP_GPIO 3

mcpwm_timer_handle_t timer = NULL;
static  mcpwm_timer_config_t timer_config = {
    .group_id = 0,
    .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
    .resolution_hz = TIMER_FREQ_HZ,
    .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    .period_ticks = TIMER_PERIOD_US,
};

static const char* TAG = "APP"; 

void app_run() {
     // init shared timer object for motors
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    // adc
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1, // just assume
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    // Pull up the sleep GPIO
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << MOTOR_SLEEP_GPIO);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // Set the sleep pin to high to enable the motor
    gpio_set_level((gpio_num_t)MOTOR_SLEEP_GPIO, 1);
    

    // create new LSM6DS3 object
    HAL_LSM6DS3 imu;
    HAL_Motor leftMotor("leftMotor");
    HAL_Motor rightMotor("rightMotor");

    imu.init(SPI3_HOST, SPI1_PIN_NUM_MISO, SPI1_PIN_NUM_MOSI, SPI1_PIN_NUM_CLK, SPI1_PIN_NUM_CS);

    // Initialize the motors
    leftMotor.init(timer, PWML_GPIO,  100.0f, &adc1_handle, CURR_SENSE_L_ADC_CHANNEL, 1.0f);
    rightMotor.init(timer, PWMR_GPIO, 100.0f, &adc1_handle, CURR_SENSE_R_ADC_CHANNEL, 1.0f);


    ESP_LOGI(TAG, "Enable and start timer");
    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));


    while(true)
    {
        imu.poll();
        leftMotor.setDutyCycle(0.6);
        leftMotor.getCurrent();
        rightMotor.setDutyCycle(0.6);
        rightMotor.getCurrent();
        // send a udp packet with the message "hello world"
        char message[] = "hello world";
        send_udp_packet(message, sizeof(message));
        run_wifi_cmds();
        vTaskDelay(pdMS_TO_TICKS(100));  // Delay to print once per tick
    }

}