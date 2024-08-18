import socket
import argparse
import time
from messages import *
import struct
from processing_utils import *
from generated.messages.header import header_pb2
from generated.messages.imu import imu_pb2
from generated.messages.telem import telem_pb2

DEFAULT_SERVER_IP = "10.0.0.203"
DEFAULT_SERVER_PORT = 5007

def process_messages(data):
    while len(data) > HEADER_SIZE_BYTES:
        header = extract_header_contents(data)
        if header.channel == header_pb2.MessageChannels.IMU_TELEM:
            message = imu_pb2.ImuTelem()
        elif header.channel == header_pb2.MessageChannels.MOTOR_TELEM:
            message = telem_pb2.MotorData()
        else:
            print(f"Unknown message type: {header.channel}")
            return
        
        message.ParseFromString(data[HEADER_SIZE_BYTES:HEADER_SIZE_BYTES + header.length])

        if header.channel == header_pb2.MessageChannels.IMU_TELEM:
            print(f"Received message of type {header.channel} at timestamp {header.timestamp} with contents: {message}")
        data = data[HEADER_SIZE_BYTES + header.length:]
    
    print("======Done processing messages==========")


def main(socket):
    while True:
        # receive a message from the server
        data, addr = sock.recvfrom(1024)
        #print(f"Timestamp: {time.time()} | Received message from server: {data}")
        process_messages(data)


if __name__ == '__main__':
    # create args for the server IP and port
    parser = argparse.ArgumentParser()
    parser.add_argument("--server_ip", type=str, default=DEFAULT_SERVER_IP)
    parser.add_argument("--server_port", type=int, default=DEFAULT_SERVER_PORT)
    args = parser.parse_args()

    SERVER_IP = args.server_ip
    SERVER_PORT = args.server_port

    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # allow the socket to be reused (just in case weird restarts or something)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    sock.sendto(b"Connected by UDP", (SERVER_IP, SERVER_PORT))
    main(sock)

