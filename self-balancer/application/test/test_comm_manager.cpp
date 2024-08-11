#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "TransportLayerMock.hpp"
#include "MessageQueueMock.hpp"
#include "CommManager.hpp"

using namespace ::testing;

TEST(ExampleTest, Example) {
    EXPECT_EQ(1, 1);
}

TEST(CommManagerTest, QueueAndSendMessage)
{
    TransportLayerMock transportLayerMock;
    MessageQueueMock messageQueueMock;

    CommManager commManager(transportLayerMock, messageQueueMock);

    // Make a dummy message for IMU_TELEM
    MessageQueue::Message message;
    message.channel = MessageChannel::IMU_TELEM;
    message.timestamp = 2;
    // Set the buffer to some dummy data
    message.length = 4;
    message.buffer[0] = 0x01;
    message.buffer[1] = 0x02;
    message.buffer[2] = 0x03;
    message.buffer[3] = 0x04;

    MessageQueue::Message message2;
    message2.channel = MessageChannel::MOTOR_TELEM;
    message2.timestamp = 0;
    message2.length = 2;
    message2.buffer[0] = 0x05;
    message2.buffer[1] = 0x06;

    EXPECT_CALL(messageQueueMock, receive(_))
    .WillOnce(DoAll(SetArgReferee<0>(message), Return(true)))
    .WillOnce(DoAll(SetArgReferee<0>(message2), Return(true)))
    .WillRepeatedly(Return(false));

    // Expect a transport layer send call, with the following buffer:
    uint8_t expectedBuffer[] = {static_cast<uint8_t>(message.channel), 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 3, 4, static_cast<uint8_t>(message2.channel), 0, 0, 0, 0, 0, 0, 0, 0, 5, 6};
    std::vector<uint8_t> sentBuffer;
    // Expect a transport layer send call with a buffer that matches the defined buffer
    EXPECT_CALL(transportLayerMock, send(_, sizeof(expectedBuffer)))
        .WillOnce(Invoke([&sentBuffer](const uint8_t* buf, int length) {
            sentBuffer.assign(buf, buf + length);
            return true;
        }));


    commManager.run();

    EXPECT_THAT(sentBuffer, ElementsAreArray(expectedBuffer));
}

// Test receive behaviour - expect a poll to the receive method of the transport layer, and the comm manager API for command processes corretly
