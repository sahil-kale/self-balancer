#ifndef MESSAGE_QUEUE_MOCK_HPP
#define MESSAGE_QUEUE_MOCK_HPP

#include <gmock/gmock.h>
#include "MessageQueue.hpp"

class MessageQueueMock : public MessageQueue {
    public:
    MOCK_METHOD1(send, bool(const Message& message));
    MOCK_METHOD1(receive, bool(Message& message));
};

#endif // MESSAGE_QUEUE_MOCK_HPP