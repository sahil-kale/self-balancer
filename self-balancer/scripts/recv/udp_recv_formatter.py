import socket
import argparse
import time
from channels import *
import struct

DEFAULT_SERVER_IP = "10.0.0.203"
DEFAULT_SERVER_PORT = 5007

def process_message(data):
    # python struct formatter: 1 byte channel, 8 bytes timestamp (big endian)
    channel, timestamp = struct.unpack(">BQ", data[:9])
    # Extract the length of the message
    msg_len = struct.unpack(">H", data[9:11])[0]
    # Extract the message
    msg = data[11:11+msg_len]
    breakpoint()


def main(socket):
    while True:
        # receive a message from the server
        data, addr = sock.recvfrom(1024)
        #print(f"Timestamp: {time.time()} | Received message from server: {data}")
        process_message(data)


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
    generate_message_sizes()
    main(sock)

