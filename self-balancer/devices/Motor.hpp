#ifndef MOTOR_HPP
#define MOTOR_HPP

class BaseMotor {
    public:
    /**
     * @brief Set the duty cycle of the motor (-1.0 to 1.0)
     * @note: -1.0 is full reverse, 1.0 is full forward. Direction is determined by the motor.
     */
    virtual void setDutyCycle(float dutyCycle) = 0;

    /**
     * @brief Get the current of the motor in Amps
     */
    virtual float getCurrent() = 0;
};

#endif // MOTOR_HPP