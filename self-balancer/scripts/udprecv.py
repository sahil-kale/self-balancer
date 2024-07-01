import socket

import struct

SERVER_IP = "192.168.1.41"
SERVER_PORT = 5007

if __name__ == '__main__':
    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # allow the socket to be reused (just in case weird restarts or something)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    while True:
        # send a message to the server (poll the user first via stdin)
        message = input("Enter a message to send to the server: ")
        sock.sendto(message.encode(), (SERVER_IP, SERVER_PORT))

        # receive a message from the server
        data, addr = sock.recvfrom(1024)
        print(f"Received message from server: {data.decode()}")

