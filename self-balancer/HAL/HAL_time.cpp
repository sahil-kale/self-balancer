#include "HAL_time.hpp"
#include "esp_timer.h"

HAL_TimeServer::HAL_TimeServer() {
}

HAL_TimeServer::~HAL_TimeServer() {
}

utime_t HAL_TimeServer::getUtimeUs() {
    return (utime_t)esp_timer_get_time();
}