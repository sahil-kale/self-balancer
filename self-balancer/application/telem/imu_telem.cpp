#include "imu_telem.hpp"

#include <pb_encode.h>
#include <string.h>

#include "messages/header/header.pb.h"
#include "messages/imu/imu.pb.h"
#include "util.hpp"

IMUTelem::IMUTelem(MessageQueue& messageQueue, BaseIMU& imu, TimeServer& timeServer)
    : messageQueue_(messageQueue), imu_(imu), timeServer_(timeServer) {}

void IMUTelem::run() {
    // Get the gyro and acceleration measurements from the IMU
    BaseIMU::Vector3D gyro = imu_.getGyro();
    BaseIMU::Vector3D accel = imu_.getAcceleration();

    // Convert to protobuf message
    ImuTelem imuTelemMessage;
    imuTelemMessage.Gyro.x_dps = gyro.x;
    imuTelemMessage.Gyro.y_dps = gyro.y;
    imuTelemMessage.Gyro.z_dps = gyro.z;
    imuTelemMessage.Accel.x_m_per_s_squared = accel.x;
    imuTelemMessage.Accel.y_m_per_s_squared = accel.y;
    imuTelemMessage.Accel.z_m_per_s_squared = accel.z;

    // Encode the message
    uint8_t buffer[ImuTelem_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&stream, ImuTelem_fields, &imuTelemMessage);

    // Send the message to the message queue
    MessageQueue::Message message;
    message.header.channel = MessageChannels_IMU_TELEM;
    message.header.timestamp = timeServer_.getUtimeUs();
    message.header.length = (uint32_t)stream.bytes_written;
    memcpy(message.buffer, buffer, message.header.length);
    messageQueue_.send(message);
}