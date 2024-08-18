#include "motor_telem.hpp"

#include <pb_encode.h>
#include <string.h>

#include "messages/header/header.pb.h"
#include "messages/telem/telem.pb.h"

MotorTelem::MotorTelem(MessageQueue& messageQueue, BaseMotor& motor, TimeServer& timeServer, MessageChannels channel)
    : messageQueue_(messageQueue), motor_(motor), timeServer_(timeServer), channel_(channel) {}

void MotorTelem::run() {
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
    message.header.channel = this->channel_;
    message.header.timestamp = timeServer_.getUtimeUs();
    message.header.length = (uint32_t)stream.bytes_written;
    memcpy(message.buffer, buffer, message.header.length);
    messageQueue_.send(message);
}