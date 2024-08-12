#include "CommManager.hpp"
#include <string.h>
#include "util.hpp"


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
        // todo: make a message serializer rather than doing this here
        datagramBuffer[bytesPopulated] = static_cast<uint8_t>(message.channel);
        bytesPopulated += 1;

        utime_t timestamp_swapped = static_cast<utime_t>(swap_endianness(message.timestamp));
        memcpy(&datagramBuffer[bytesPopulated], &timestamp_swapped, sizeof(utime_t));
        bytesPopulated += sizeof(utime_t);

        memcpy(&datagramBuffer[bytesPopulated], message.buffer, message.length);
        bytesPopulated += message.length;
    }

    if (bytesPopulated > 0)
    {
        transportLayer.send(datagramBuffer, bytesPopulated);
    }
}
