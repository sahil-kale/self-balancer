#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "motor_telem.hpp"
#include "MessageQueueMock.hpp"
#include "MotorMock.hpp"
#include "TimeServerMock.hpp"

#include <pb_encode.h>
#include <pb_decode.h>
#include "messages/telem/telem.pb.h"

using namespace ::testing;

TEST(MotorTelemTest, VerifymotorMessageConstruction) {
    MotorMock motorMock;
    MessageQueueMock messageQueueMock;
    TimeServerMock timeServerMock;
    MotorTelem motorTelem(messageQueueMock, motorMock, timeServerMock);

    // Expect the motor to be polled for the current and duty cycle measurements
    EXPECT_CALL(motorMock, getCurrent())
        .WillOnce(Return(1.0));
    
    EXPECT_CALL(motorMock, getDutyCycle())
        .WillOnce(Return(0.5));

    // Construct the sample message that we should expect to be sent
    MotorData motorTelemMessage;
    motorTelemMessage.current = 1.0;
    motorTelemMessage.dutyCycle = 0.5;

    // encode via protobuf to a buffer
    uint8_t buffer[MotorData_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&stream, MotorData_fields, &motorTelemMessage);

    // Verify the message was sent to the message queue - should store the buffer to 2 vectors, one for gyro and one for accel
    // and expect the call twice
    MessageQueue::Message message;
    EXPECT_CALL(messageQueueMock, send(_))
        .WillRepeatedly(DoAll(SaveArg<0>(&message), Return(true)));

    // Expect a call to the time server to get the current time, return a timestamp of 64
    utime_t testTimestamp = 64;
    EXPECT_CALL(timeServerMock, getUtimeUs())
        .WillOnce(Return(testTimestamp));

    motorTelem.run();

    // Verify the message is the same as the expected message
    EXPECT_EQ(message.channel, MessageChannel::MOTOR_TELEM);
    EXPECT_EQ(message.timestamp, testTimestamp);
    EXPECT_EQ(message.length, sizeof(buffer));
    // Expect that the buffers are equal up to the length of the buffer
    EXPECT_THAT(std::vector<uint8_t>(message.buffer, message.buffer + message.length), ElementsAreArray(buffer));
}
