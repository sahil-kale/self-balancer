#include "CommManager.hpp"
#include <string.h>
#include "util.hpp"

#include <pb_encode.h>
#include <pb_decode.h>
#include "messages/header/header.pb.h"

CommManager::CommManager(TransportLayer& transportLayer, MessageQueue& messageQueue)
    : transportLayer(transportLayer), messageQueue(messageQueue)
{
}

void CommManager::init()
{
    // don't do anything 
}

void CommManager::run()
{
    size_t bytesPopulated = 0U;
    MessageQueue::Message message;
    while (messageQueue.receive(message))
    {
        MessageHeader header;
        header.channel = static_cast<uint32_t>(message.channel);
        header.timestamp = static_cast<uint32_t>(message.timestamp);
        header.length = static_cast<uint32_t>(message.length);

        const size_t size_to_write = MessageHeader_size + message.length;
        if (bytesPopulated + size_to_write > MAX_DATAGRAM_BUF_SIZE)
        {
            transportLayer.send(datagramBuffer, bytesPopulated);
            bytesPopulated = 0U;
        }

        uint8_t headerBuffer[MessageHeader_size] = {0};
        pb_ostream_t headerStream = pb_ostream_from_buffer(headerBuffer, sizeof(headerBuffer));
        pb_encode(&headerStream, MessageHeader_fields, &header);

        memcpy(&datagramBuffer[bytesPopulated], headerBuffer, sizeof(headerBuffer));
        bytesPopulated += sizeof(headerBuffer);

        memcpy(&datagramBuffer[bytesPopulated], message.buffer, message.length);
        bytesPopulated += message.length;
    }

    if (bytesPopulated > 0)
    {
        transportLayer.send(datagramBuffer, bytesPopulated);
    }

    bytesPopulated = 0U;
}
