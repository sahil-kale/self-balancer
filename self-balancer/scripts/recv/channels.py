import sys
import os

import sys
import os

# Get the absolute path of the parent directory of 'generated'
parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../'))

# Add the parent directory to sys.path
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)

from enum import Enum
from generated.messages.imu import imu_pb2
from generated.messages.telem import telem_pb2

class MessageChannel(Enum):
    IMU_TELEM = 0
    MOTOR_TELEM = 1
    NUM_CHANNELS = 2

message_mapping = {
    MessageChannel.IMU_TELEM: imu_pb2.ImuTelem,
    MessageChannel.MOTOR_TELEM: telem_pb2.MotorData,
}
