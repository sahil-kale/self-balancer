#include "HAL_messageQueue.hpp"

#include "esp_log.h"
#include "util.hpp"

HAL_MessageQueue::HAL_MessageQueue(uint8_t ucQueueStorage[QUEUE_STORAGE_SIZE_BYTES]) : ucQueueStorage(ucQueueStorage) {
    // Create the queue
    this->xQueue = xQueueCreateStatic(MAX_MESSAGE_QUEUE_SIZE, BYTES_PER_QUEUE_ITEM, this->ucQueueStorage, &this->xStaticQueue);
    configASSERT(this->xQueue);
}

HAL_MessageQueue::~HAL_MessageQueue() { vQueueDelete(this->xQueue); }

bool HAL_MessageQueue::send(const Message& message) {
    const bool success = xQueueSend(this->xQueue, &message, (TickType_t)0U) == pdPASS;
    if (!success) {
        ESP_LOGE("HAL_MessageQueue", "Failed to send message");
    }
    return success;
}

bool HAL_MessageQueue::receive(Message& message) {
    // Receive the message
    return xQueueReceive(this->xQueue, &message, (TickType_t)0U) == pdPASS;
}