#include "HAL_messageQueue.hpp"
#include "util.hpp"
#include "esp_log.h"

HAL_MessageQueue::HAL_MessageQueue(uint8_t ucQueueStorage[QUEUE_STORAGE_SIZE_BYTES])
    : ucQueueStorage(ucQueueStorage)
{
    // Create the queue
    this->xQueue = xQueueCreateStatic(MAX_MESSAGE_QUEUE_SIZE, BYTES_PER_QUEUE_ITEM, this->ucQueueStorage, &this->xStaticQueue);
    configASSERT(this->xQueue);
}

HAL_MessageQueue::~HAL_MessageQueue()
{
    vQueueDelete(this->xQueue);
}

bool HAL_MessageQueue::send(const Message& message)
{
    return xQueueSend(this->xQueue, &message, (TickType_t)0U) == pdPASS;
}

bool HAL_MessageQueue::receive(Message& message)
{
    // Receive the message
    return xQueueReceive(this->xQueue, &message, (TickType_t)0U) == pdPASS;
}