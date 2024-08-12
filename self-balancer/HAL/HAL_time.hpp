#ifndef HAL_TIME_HPP
#define HAL_TIME_HPP
#include "time.hpp"

class HAL_TimeServer : public TimeServer {
    public:
    HAL_TimeServer();
    virtual ~HAL_TimeServer();

    utime_t getUtimeUs() override;
};

#endif // HAL_TIME_HPP