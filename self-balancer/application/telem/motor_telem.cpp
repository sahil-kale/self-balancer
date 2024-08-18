#include "motor_telem.hpp"
#include <string.h>
#include <pb_encode.h>
#include "messages/telem/telem.pb.h"
#include "messages/header/header.pb.h"

MotorTelem::MotorTelem(MessageQueue& messageQueue, BaseMotor& motor, TimeServer& timeServer) : messageQueue_(messageQueue), motor_(motor), timeServer_(timeServer) {}

void MotorTelem::run()
{
    // Get the current and duty cycle from the motor
    float current = motor_.getCurrent();
    float dutyCycle = motor_.getDutyCycle();

    // Construct the message
    MotorData motorTelemMessage;
    motorTelemMessage.current = current;
    motorTelemMessage.dutyCycle = dutyCycle;

    // Encode the message
    uint8_t buffer[MotorData_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&stream, MotorData_fields, &motorTelemMessage);

    // Send the message
    MessageQueue::Message message;
    message.header.channel = MessageChannels_MOTOR_TELEM;
    message.header.timestamp = timeServer_.getUtimeUs();
    message.header.length = sizeof(buffer);
    memcpy(message.buffer, buffer, sizeof(buffer));
    messageQueue_.send(message);
}