import socket
import argparse
import time
from messages import *
import struct
from processing_utils import *
from generated.messages.header import header_pb2
from generated.messages.imu import imu_pb2
from generated.messages.telem import telem_pb2
from google.protobuf.json_format import MessageToJson

from kalman_filter_test import *

DEFAULT_SERVER_IP = "10.0.0.202"
DEFAULT_SERVER_PORT = 5007

kf = KalmanFilter()

def process_messages(data, log_file, print_msgs=False):
    while len(data) > HEADER_SIZE_BYTES:
        header, message = get_header_and_message(data)
        if header.channel == header_pb2.MessageChannels.IMU_TELEM:
            message = imu_pb2.ImuTelem()
        elif header.channel == header_pb2.MessageChannels.MOTOR_L_TELEM:
            message = telem_pb2.MotorData()
        elif header.channel == header_pb2.MessageChannels.MOTOR_R_TELEM:
            message = telem_pb2.MotorData()
        else:
            print(f"Unknown message type: {header.channel}")
            return

        message.ParseFromString(
            data[HEADER_SIZE_BYTES : HEADER_SIZE_BYTES + header.length]
        )
        data = data[HEADER_SIZE_BYTES + header.length :]

        # log the message to a file
        if log_file:
            log_message_to_file(header, message, log_file)
        if print_msgs:
            print(f"Timestamp: {header.timestamp}")
            print(message)

        if header.channel == header_pb2.MessageChannels.IMU_TELEM:
            kf.run(message)


def log_message_to_file(header, message, file_obj):
    header_json = MessageToJson(header, indent=2)
    message_json = MessageToJson(message, indent=2)

    # Write the formatted JSON object to a file
    file_obj.write(f'{{\n  "header": {header_json},\n  "message": {message_json}\n}},')


def main(sock, log_file, args):
    while True:
        # receive a message from the server
        data, addr = sock.recvfrom(65535)
        # print(f"Timestamp: {time.time()} | Received message from server: {data}")
        process_messages(data, log_file, print_msgs=args.print_msgs)


if __name__ == "__main__":
    # create args for the server IP and port
    parser = argparse.ArgumentParser()
    parser.add_argument("--server_ip", type=str, default=DEFAULT_SERVER_IP)
    parser.add_argument("--server_port", type=int, default=DEFAULT_SERVER_PORT)
    parser.add_argument(
        "--log_file", type=str, default=f"logs/dumplog_{time.time()}.json"
    )
    parser.add_argument("--print_msgs", action="store_true")
    args = parser.parse_args()

    SERVER_IP = args.server_ip
    SERVER_PORT = args.server_port

    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # allow the socket to be reused (just in case weird restarts or something)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    sock.sendto(b"Connected by UDP", (SERVER_IP, SERVER_PORT))

    # make a log dir
    os.makedirs("logs", exist_ok=True)

    # create a log json file with the title being dumplog and the current time. Store in the logs directory
    log_file = open(args.log_file, "w")
    # write the opening bracket for the json array
    log_file.write("[\n")
    try:
        main(sock, log_file, args)
    except KeyboardInterrupt:
        # delete the last comma in the json array
        log_file.seek(log_file.tell() - 1)
        log_file.truncate()

        # write the closing bracket for the json array
        log_file.write("\n]\n")
        log_file.close()
