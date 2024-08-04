#include "HAL_Motor.hpp"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

HAL_Motor::HAL_Motor(const char* tag) : TAG(tag) {
    currentConversionFactor = 0.0f;
    current = 0.0f;
}

HAL_Motor::~HAL_Motor() {
}

void HAL_Motor::init(mcpwm_timer_handle_t timer, uint8_t pwmGpioNum, float dutyCycleToTicksConversion, adc_oneshot_unit_handle_t* adcHandle, adc_channel_t adcChannel, float currentConversionFactor) {
    this->currentConversionFactor = currentConversionFactor;
    this->dutyCycleToTicksConversion = dutyCycleToTicksConversion;
    this->adcHandle = adcHandle;
    this->adcChannel = adcChannel;

    mcpwm_oper_handle_t oper = NULL;
    mcpwm_operator_config_t operator_config = {
        .group_id = 0, // operator must be in the same group to the timer
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &oper));
    ESP_LOGI(TAG, "Connect timer and operator");
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

    mcpwm_comparator_config_t comparator_config = {
        .flags = 
        {
            .update_cmp_on_tez = true, // update when timer counts to 0
        }
    };

    comparatorHandle = NULL;
    ESP_ERROR_CHECK(mcpwm_new_comparator(oper, &comparator_config, &comparatorHandle));

    mcpwm_gen_handle_t generator = NULL;
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = pwmGpioNum,
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &generator_config, &generator));

    setDutyCycle(0.0f);

    // go high on counter empty
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generator,
                                                              MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    // go low on compare threshold
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generator,
                                                                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparatorHandle, MCPWM_GEN_ACTION_LOW)));

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };

    // Set up the ADC channel
    ESP_ERROR_CHECK(adc_oneshot_config_channel(*adcHandle, adcChannel, &config));
    // configure voltage conversion factor
    voltageConversionFactor = (3.1f / (1 << config.bitwidth));

}

void HAL_Motor::setDutyCycle(float dutyCycle) {
    const uint32_t ticks = (uint32_t)(dutyCycle * dutyCycleToTicksConversion);
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparatorHandle, ticks));
}

float HAL_Motor::getCurrent() {
    // Read the current of the motor
    int dout;
    ESP_ERROR_CHECK(adc_oneshot_read(*adcHandle, adcChannel, &dout));
    const float voltage = dout * voltageConversionFactor;
    current = voltage * currentConversionFactor;
    return current;
}
