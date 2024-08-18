from messages import *

# generate the HEADER_SIZE_BYTES constant by serializing a test header
def generate_header_size_bytes():
    testHeader = header_pb2.MessageHeader()
    testHeader.channel = 1
    testHeader.timestamp = 2
    testHeader.length = 1234
    test = testHeader.SerializeToString()
    return len(test)

HEADER_SIZE_BYTES = generate_header_size_bytes()

def extract_header_contents(data):
    header = header_pb2.MessageHeader()
    header.ParseFromString(data[:HEADER_SIZE_BYTES])
    return header
