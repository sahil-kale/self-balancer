#ifndef HAL_MESSAGE_QUEUE_HPP
#define HAL_MESSAGE_QUEUE_HPP

#include "MessageQueue.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define MAX_MESSAGE_QUEUE_SIZE 200

class HAL_MessageQueue : public MessageQueue {
    public:
    HAL_MessageQueue(uint8_t ucQueueStorage[MAX_MESSAGE_QUEUE_SIZE * MAX_MESSAGE_BUF_BYTES]);
    ~HAL_MessageQueue();

    bool send(const Message& message) override;
    bool receive(Message& message) override;

    private:
    StaticQueue_t xStaticQueue;
    QueueHandle_t xQueue;
    uint8_t* ucQueueStorage = nullptr;
};


#endif // HAL_MESSAGE_QUEUE_HPP