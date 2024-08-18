#include <pb_decode.h>
#include <pb_encode.h>

#include "IMUMock.hpp"
#include "MessageQueueMock.hpp"
#include "TimeServerMock.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "imu_telem.hpp"
#include "messages/imu/imu.pb.h"

using namespace ::testing;

TEST(IMUTelemTest, VerifyImuMessageConstruction) {
    IMUMock imuMock;
    MessageQueueMock messageQueueMock;
    TimeServerMock timeServerMock;
    IMUTelem imuTelem(messageQueueMock, imuMock, timeServerMock);

    // Expect the IMU to be polled for the gyro and accel measurements
    EXPECT_CALL(imuMock, getGyro()).WillOnce(Return(BaseIMU::Vector3D{1.0, 2.0, 3.0, 0, true}));
    EXPECT_CALL(imuMock, getAcceleration()).WillOnce(Return(BaseIMU::Vector3D{4.0, 5.0, 6.0, 0, true}));

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

    MessageQueue::Message message;
    EXPECT_CALL(messageQueueMock, send(_)).WillRepeatedly(DoAll(SaveArg<0>(&message), Return(true)));

    // Expect a call to the time server to get the current time, return a timestamp of 64
    utime_t testTimestamp = 64;
    EXPECT_CALL(timeServerMock, getUtimeUs()).WillOnce(Return(testTimestamp));

    imuTelem.run();

    // Verify the message is the same as the expected message
    EXPECT_EQ(message.header.channel, MessageChannels_IMU_TELEM);
    EXPECT_EQ(message.header.timestamp, testTimestamp);
    EXPECT_EQ(message.header.length, stream.bytes_written);
    // Expect that the buffers are equal up to the length of the buffer
    EXPECT_THAT(std::vector<uint8_t>(message.buffer, message.buffer + message.header.length), ElementsAreArray(buffer));
}
