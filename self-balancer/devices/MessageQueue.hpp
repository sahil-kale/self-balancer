#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP
#include <stdint.h>
#include <stddef.h>
#include "channels.hpp"
#include "time.hpp"

#define MAX_MESSAGE_BUF_BYTES 64
#define MAX_MESSAGE_QUEUE_SIZE 50
#define BYTES_PER_QUEUE_ITEM (sizeof(MessageQueue::Message))
#define QUEUE_STORAGE_SIZE_BYTES (MAX_MESSAGE_QUEUE_SIZE * BYTES_PER_QUEUE_ITEM)

class MessageQueue {
    public:
    typedef struct {
        MessageChannel channel;
        utime_t timestamp;
        size_t length;
        uint8_t buffer[MAX_MESSAGE_BUF_BYTES];
    } Message;

    virtual bool send(const Message& message) = 0;
    virtual bool receive(Message& message) = 0;
};

#endif