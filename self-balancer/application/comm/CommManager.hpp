#ifndef COMM_MANAGER_HPP
#define COMM_MANAGER_HPP
#include "TransportLayer.hpp"
#include "MessageQueue.hpp"
#define MAX_DATAGRAM_BUF_SIZE (QUEUE_STORAGE_SIZE_BYTES)

class CommManager {
    public:
    CommManager(TransportLayer& transport_layer, MessageQueue& message_queue);

    void init();
    void run();

    private:
    TransportLayer& transportLayer;
    MessageQueue& messageQueue;
    uint8_t datagramBuffer[MAX_DATAGRAM_BUF_SIZE];
};

#endif // COMM_MANAGER_HPP