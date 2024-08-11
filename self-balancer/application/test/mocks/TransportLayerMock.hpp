#ifndef TRANSPORT_LAYER_MOCK_HPP
#define TRANSPORT_LAYER_MOCK_HPP

#include <gmock/gmock.h>
#include "TransportLayer.hpp"

class TransportLayerMock : public TransportLayer {
    public:
    MOCK_METHOD2(send, bool(const uint8_t* buffer, size_t length));
    MOCK_METHOD2(receive, size_t(uint8_t* buffer, size_t length));
};

#endif // TRANSPORT_LAYER_MOCK_HPP
