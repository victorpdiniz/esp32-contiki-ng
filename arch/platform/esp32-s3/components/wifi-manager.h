/*
 * Copyright (c) 2025, Contiki-NG ESP32-S3 Port
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *         WiFi Manager for ESP32-S3
 * \author
 *         Contiki-NG ESP32-S3 Port
 * 
 * \brief WiFi management using ESP-IDF APIs with Contiki integration
 */

#ifndef WIFI_MANAGER_H_
#define WIFI_MANAGER_H_

#include "contiki.h"
#include "esp_wifi.h"
#include "esp_event.h"

/*---------------------------------------------------------------------------*/
/* WiFi events for Contiki processes */
extern process_event_t wifi_event_connected;
extern process_event_t wifi_event_disconnected;
extern process_event_t wifi_event_got_ip;

/*---------------------------------------------------------------------------*/
/* WiFi status */
typedef enum {
  WIFI_STATUS_IDLE,
  WIFI_STATUS_CONNECTING,
  WIFI_STATUS_CONNECTED,
  WIFI_STATUS_DISCONNECTED,
  WIFI_STATUS_ERROR
} wifi_status_t;

/*---------------------------------------------------------------------------*/
/* WiFi connection info */
typedef struct {
  char ssid[32];
  uint8_t bssid[6];
  int8_t rssi;
  uint8_t channel;
  uint32_t ip_addr;
} wifi_info_t;

/*---------------------------------------------------------------------------*/
/**
 * \brief Initialize WiFi manager
 * 
 * Sets up WiFi in station mode and registers event handlers
 */
void wifi_manager_init(void);

/**
 * \brief Connect to WiFi access point
 * 
 * \param ssid SSID of the access point
 * \param password Password (NULL for open networks)
 * 
 * \return 0 on success, -1 on failure
 */
int wifi_manager_connect(const char *ssid, const char *password);

/**
 * \brief Disconnect from WiFi
 */
void wifi_manager_disconnect(void);

/**
 * \brief Get current WiFi status
 * 
 * \return Current WiFi status
 */
wifi_status_t wifi_manager_get_status(void);

/**
 * \brief Get WiFi connection information
 * 
 * \param info Pointer to wifi_info_t structure to fill
 * 
 * \return 0 on success, -1 if not connected
 */
int wifi_manager_get_info(wifi_info_t *info);

/**
 * \brief Get RSSI (signal strength)
 * 
 * \return RSSI in dBm, or 0 if not connected
 */
int8_t wifi_manager_get_rssi(void);

/*---------------------------------------------------------------------------*/
#endif /* WIFI_MANAGER_H_ */
