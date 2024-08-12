#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "imu_telem.hpp"
#include "MessageQueueMock.hpp"
#include "IMUMock.hpp"
#include "TimeServerMock.hpp"

#include <pb_encode.h>
#include <pb_decode.h>
#include "messages/imu/imu.pb.h"

using namespace ::testing;

TEST(IMUTelemTest, VerifyImuMessageConstruction) {
    IMUMock imuMock;
    MessageQueueMock messageQueueMock;
    TimeServerMock timeServerMock;
    IMUTelem imuTelem(messageQueueMock, imuMock, timeServerMock);

    // Expect the IMU to be polled for the gyro and accel measurements
    EXPECT_CALL(imuMock, getGyro())
        .WillOnce(Return(BaseIMU::Vector3D{1.0, 2.0, 3.0, 0, true}));
    EXPECT_CALL(imuMock, getAcceleration())
        .WillOnce(Return(BaseIMU::Vector3D{4.0, 5.0, 6.0, 0, true}));

    // Construct the sample message that we should expect to be sent
    ImuTelem imuTelemMessage;
    imuTelemMessage.Gyro.x_dps = 1.0;
    imuTelemMessage.Gyro.y_dps = 2.0;
    imuTelemMessage.Gyro.z_dps = 3.0;
    imuTelemMessage.Accel.x_m_per_s_squared = 4.0;
    imuTelemMessage.Accel.y_m_per_s_squared = 5.0;
    imuTelemMessage.Accel.z_m_per_s_squared = 6.0;

    // encode via protobuf to a buffer
    uint8_t buffer[ImuTelem_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&stream, ImuTelem_fields, &imuTelemMessage);

    // Verify the message was sent to the message queue - should store the buffer to 2 vectors, one for gyro and one for accel
    // and expect the call twice
    MessageQueue::Message message;
    EXPECT_CALL(messageQueueMock, send(_))
        .WillRepeatedly(DoAll(SaveArg<0>(&message), Return(true)));

    // Expect a call to the time server to get the current time, return a timestamp of 64
    utime_t testTimestamp = 64;
    EXPECT_CALL(timeServerMock, getUtimeUs())
        .WillOnce(Return(testTimestamp));

    imuTelem.run();

    // Verify the message is the same as the expected message
    EXPECT_EQ(message.channel, MessageChannel::IMU_TELEM);
    EXPECT_EQ(message.timestamp, testTimestamp);
    EXPECT_EQ(message.length, sizeof(buffer));
    // Expect that the buffers are equal up to the length of the buffer
    EXPECT_THAT(std::vector<uint8_t>(message.buffer, message.buffer + message.length), ElementsAreArray(buffer));
}
