#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "TransportLayerMock.hpp"
#include "MessageQueueMock.hpp"
#include "CommManager.hpp"

#include <pb_encode.h>
#include <pb_decode.h>
#include "messages/header/header.pb.h"

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
    message.header.channel = MessageChannels_IMU_TELEM;
    message.header.timestamp = 0;
    message.header.length = 4;
    message.buffer[0] = 0x01;
    message.buffer[1] = 0x02;
    message.buffer[2] = 0x03;
    message.buffer[3] = 0x04;

    // seralize the header
    uint8_t headerBuffer[MessageHeader_size] = {0};
    pb_ostream_t headerStream = pb_ostream_from_buffer(headerBuffer, sizeof(headerBuffer));
    pb_encode(&headerStream, MessageHeader_fields, &message.header);

    MessageQueue::Message message2;
    message2.header.channel = MessageChannels_MOTOR_L_TELEM;
    message2.header.timestamp = 0;
    message2.header.length = 2;
    message2.buffer[0] = 0x05;
    message2.buffer[1] = 0x06;

    // seralize the header
    uint8_t headerBuffer2[MessageHeader_size] = {0};
    pb_ostream_t headerStream2 = pb_ostream_from_buffer(headerBuffer2, sizeof(headerBuffer2));
    pb_encode(&headerStream2, MessageHeader_fields, &message2.header);

    EXPECT_CALL(messageQueueMock, receive(_))
    .WillOnce(DoAll(SetArgReferee<0>(message), Return(true)))
    .WillOnce(DoAll(SetArgReferee<0>(message2), Return(true)))
    .WillRepeatedly(Return(false));

    // Expect a transport layer send call, with the following buffer:
    

    // Create expected buffer
    uint8_t expectedBuffer[MessageHeader_size + 4 + MessageHeader_size + 2];
    memcpy(expectedBuffer, headerBuffer, MessageHeader_size);
    memcpy(expectedBuffer + MessageHeader_size, message.buffer, 4);
    memcpy(expectedBuffer + MessageHeader_size + 4, headerBuffer2, MessageHeader_size);
    memcpy(expectedBuffer + MessageHeader_size + 4 + MessageHeader_size, message2.buffer, 2);

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

#if 0
// Test receive behaviour - expect a poll to the receive method of the transport layer, and the comm manager API for command processes corretly
TEST(CommManagerTest, ReceiveAndVerifyCommandApiWorks)
{
    TransportLayerMock transportLayerMock;
    MessageQueueMock messageQueueMock;

    CommManager commManager(transportLayerMock, messageQueueMock);

    // Expect a transport layer receive call, with the following message seralized:

}
#endif