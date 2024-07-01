/**
 * @file wifi_cmds.h
 * This code taken from: 
 * https://github.com/sahil-kale/Wireless-Soil-Moisture-Sensor/blob/main/main/src/wifi_cmds.c
 * Originally taken from: UDP Multicast Ping Example
 * 
 * 
 * 
 */

#ifndef WIFI_CMDS_H
#define WIFI_CMDS_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Starts the wifi peripheral
 *
 * @returns error code - 0 = successful, 1 = failure, 2 = unexpected event
 */
uint8_t wifi_init_sta(void);

/** @brief Send UDP Packet command
 *
 * @param buffer pointer to the buffer to send
 * @param len length of the buffer to read in bytes
 *
 */
int8_t send_udp_packet(void *buffer, size_t len);

/**
 * @brief function to reconnect to wifi if the system gets disconnected
 *
 */
uint8_t reconnect_wifi();

void run_wifi_cmds(void);

#endif