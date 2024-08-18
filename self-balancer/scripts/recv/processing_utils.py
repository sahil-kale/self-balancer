import os
import sys

# Get the absolute path of the parent directory of 'generated'
parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../'))

# Add the parent directory to sys.path
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)

from enum import Enum
from generated.messages.header import header_pb2

def extract_header_contents(data):
    header = header_pb2.MessageHeader()
    testHeader = header_pb2.MessageHeader()
    testHeader.channel = 1
    testHeader.timestamp = 2
    testHeader.length = 1234
    breakpoint()
    header.ParseFromString(data)
    return header
