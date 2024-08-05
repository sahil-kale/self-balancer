#ifndef HAL_WIFI_HPP
#define HAL_WIFI_HPP
#include "TransportLayer.hpp"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

class HAL_Wifi : public TransportLayer {
    public:
    HAL_Wifi();
    virtual ~HAL_Wifi();

    void init();
    void run();

    bool send(const uint8_t* buffer, size_t length);
    size_t receive(uint8_t* buffer, size_t length);

    static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);


    private:
    void init_udp_server_task(void);
        
    int8_t sock = 0;
    struct sockaddr_storage source_addr;
    esp_netif_t *netif_interface;
    struct sockaddr_in6 dest_addr;
    bool client_connected = false;
};

#endif // HAL_WIFI_HPP