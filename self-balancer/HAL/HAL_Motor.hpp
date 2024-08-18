#ifndef HAL_MOTOR_HPP
#define HAL_MOTOR_HPP

#include <stdint.h>

#include "Motor.hpp"
#include "driver/mcpwm_prelude.h"
#include "esp_adc/adc_oneshot.h"

class HAL_Motor : public BaseMotor {
   public:
    HAL_Motor(const char* tag);
    virtual ~HAL_Motor();

    void init(mcpwm_timer_handle_t timer, uint8_t pwmGpioNum, float dutyCycleToTicksConversion,
              adc_oneshot_unit_handle_t* adcHandle, adc_channel_t adcChannel, float currentConversionFactor);

    /**
     * @brief Set the duty cycle of the motor (-1.0 to 1.0)
     * @note: -1.0 is full reverse, 1.0 is full forward. Direction is determined by the motor.
     */
    void setDutyCycle(float dutyCycle) override;

    /**
     * @brief Get the current of the motor in Amps
     */
    float getCurrent() override;

    /**
     * @brief Get the duty cycle of the motor (-1.0 to 1.0)
     * @note: -1.0 is full reverse, 1.0 is full forward. Direction is determined by the motor.
     */
    float getDutyCycle() override;

   private:
    float voltageConversionFactor;
    float currentConversionFactor;
    float current;
    mcpwm_cmpr_handle_t comparatorHandle;
    float dutyCycleToTicksConversion;
    const char* TAG;
    adc_oneshot_unit_handle_t* adcHandle;
    adc_channel_t adcChannel;

    float dutyCycle_;
};

#endif  // HAL_MOTOR_HPP