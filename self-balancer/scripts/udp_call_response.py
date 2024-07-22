import socket
import argparse

DEFAULT_SERVER_IP = "192.168.1.41"
DEFAULT_SERVER_PORT = 5007

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

    while True:
        # send a message to the server (poll the user first via stdin)
        message = input("Enter a message to send to the server: ")
        sock.sendto(message.encode(), (SERVER_IP, SERVER_PORT))

        # receive a message from the server
        data, addr = sock.recvfrom(1024)
        print(f"Received message from server: {data.decode()}")

