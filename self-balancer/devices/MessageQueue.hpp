#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP
#include <stdint.h>
#include <stddef.h>
#include "time.hpp"
#include "messages/header/header.pb.h"

#define MAX_MESSAGE_BUF_BYTES 64
#define MAX_MESSAGE_QUEUE_SIZE 200
#define BYTES_PER_QUEUE_ITEM (sizeof(MessageQueue::Message))
#define QUEUE_STORAGE_SIZE_BYTES (MAX_MESSAGE_QUEUE_SIZE * BYTES_PER_QUEUE_ITEM)

class MessageQueue {
    public:
    typedef struct {
        MessageHeader header;
        uint8_t buffer[MAX_MESSAGE_BUF_BYTES];
    } Message;

    virtual bool send(const Message& message) = 0;
    virtual bool receive(Message& message) = 0;
};

#endif