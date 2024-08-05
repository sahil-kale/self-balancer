#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP
#include <stdint.h>
#include <stddef.h>
#include "channels.hpp"

#define MAX_MESSAGE_BUF_BYTES 64

class MessageQueue {
    public:
    typedef struct {
        MessageChannel channel;
        size_t length;
        uint8_t buffer[MAX_MESSAGE_BUF_BYTES];
    } Message;

    virtual bool send(const Message& message) = 0;
    virtual bool receive(Message& message) = 0;
};

#endif