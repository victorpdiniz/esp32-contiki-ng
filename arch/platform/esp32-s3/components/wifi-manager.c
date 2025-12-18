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
 *         WiFi Manager Implementation
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#include "wifi-manager.h"
#include "freertos-bridge.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include <string.h>

static const char *TAG = "WiFi-Manager";

/* WiFi status */
static wifi_status_t wifi_status = WIFI_STATUS_IDLE;
static wifi_info_t current_wifi_info;

/* Event handles */
static esp_event_handler_instance_t instance_any_id;
static esp_event_handler_instance_t instance_got_ip;

/* Process events */
process_event_t wifi_event_connected;
process_event_t wifi_event_disconnected;
process_event_t wifi_event_got_ip;

/* Process to receive WiFi events */
PROCESS(wifi_event_process, "WiFi Event Process");

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(wifi_event_process, ev, data)
{
  PROCESS_BEGIN();
  
  while(1) {
    PROCESS_WAIT_EVENT();
    
    if(ev == wifi_event_connected) {
      ESP_LOGI(TAG, "Contiki process received: WiFi connected event");
    } else if(ev == wifi_event_disconnected) {
      ESP_LOGI(TAG, "Contiki process received: WiFi disconnected event");
    } else if(ev == wifi_event_got_ip) {
      wifi_info_t *info = (wifi_info_t *)data;
      if(info != NULL) {
        ESP_LOGI(TAG, "Contiki process received: Got IP: %d.%d.%d.%d",
                 (int)(info->ip_addr & 0xFF),
                 (int)((info->ip_addr >> 8) & 0xFF),
                 (int)((info->ip_addr >> 16) & 0xFF),
                 (int)((info->ip_addr >> 24) & 0xFF));
        free(data);
      }
    }
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static void
wifi_event_handler(void* arg, esp_event_base_t event_base,
                   int32_t event_id, void* event_data)
{
  if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    ESP_LOGI(TAG, "WiFi station started");
    wifi_status = WIFI_STATUS_CONNECTING;
    esp_wifi_connect();
    
  } else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
    wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t *)event_data;
    ESP_LOGI(TAG, "Connected to AP SSID:%s channel:%d",
             event->ssid, event->channel);
    
    wifi_status = WIFI_STATUS_CONNECTED;
    
    /* Update connection info */
    memcpy(current_wifi_info.ssid, event->ssid, sizeof(current_wifi_info.ssid));
    memcpy(current_wifi_info.bssid, event->bssid, 6);
    current_wifi_info.channel = event->channel;
    
    /* Post event to Contiki */
    freertos_post_to_contiki(&wifi_event_process, 
                            wifi_event_connected,
                            NULL, 0);
    
  } else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;
    ESP_LOGI(TAG, "Disconnected from AP, reason: %d", event->reason);
    
    wifi_status = WIFI_STATUS_DISCONNECTED;
    
    /* Retry connection */
    esp_wifi_connect();
    
    /* Post event to Contiki */
    freertos_post_to_contiki(&wifi_event_process,
                            wifi_event_disconnected,
                            NULL, 0);
    
  } else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
    
    /* Update IP address */
    current_wifi_info.ip_addr = event->ip_info.ip.addr;
    
    /* Allocate memory for event data */
    wifi_info_t *info = malloc(sizeof(wifi_info_t));
    if(info != NULL) {
      memcpy(info, &current_wifi_info, sizeof(wifi_info_t));
      
      /* Post event to Contiki with IP info */
      freertos_post_to_contiki(&wifi_event_process,
                              wifi_event_got_ip,
                              info, sizeof(wifi_info_t));
    }
  }
}
/*---------------------------------------------------------------------------*/
void
wifi_manager_init(void)
{
  /* Allocate Contiki events */
  wifi_event_connected = process_alloc_event();
  wifi_event_disconnected = process_alloc_event();
  wifi_event_got_ip = process_alloc_event();
  
  /* Start WiFi event process */
  process_start(&wifi_event_process, NULL);
  
  /* Initialize network interface */
  ESP_ERROR_CHECK(esp_netif_init());
  
  /* Create default event loop, but ignore if already created */
  esp_err_t err = esp_event_loop_create_default();
  if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
    ESP_ERROR_CHECK(err);
  }
  
  /* Network interface may be created by platform init,
     so we create it only if not already present */
  esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
  if (netif == NULL) {
    esp_netif_create_default_wifi_sta();
  }
  
  /* Initialize WiFi with default config */
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  
  /* Register event handlers */
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &wifi_event_handler,
                                                      NULL,
                                                      &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                      IP_EVENT_STA_GOT_IP,
                                                      &wifi_event_handler,
                                                      NULL,
                                                      &instance_got_ip));
  
  /* Set WiFi mode to station */
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  
  ESP_LOGI(TAG, "WiFi Manager initialized");
}
/*---------------------------------------------------------------------------*/
int
wifi_manager_connect(const char *ssid, const char *password)
{
  wifi_config_t wifi_config = {0};
  
  if(ssid == NULL) {
    ESP_LOGE(TAG, "SSID cannot be NULL");
    return -1;
  }
  
  /* Configure WiFi */
  strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
  
  if(password != NULL) {
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
  }
  
  wifi_config.sta.threshold.authmode = (password != NULL) ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN;
  
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
  
  ESP_LOGI(TAG, "Connecting to SSID: %s", ssid);
  
  return 0;
}
/*---------------------------------------------------------------------------*/
void
wifi_manager_disconnect(void)
{
  esp_wifi_disconnect();
  wifi_status = WIFI_STATUS_DISCONNECTED;
}
/*---------------------------------------------------------------------------*/
wifi_status_t
wifi_manager_get_status(void)
{
  return wifi_status;
}
/*---------------------------------------------------------------------------*/
int
wifi_manager_get_info(wifi_info_t *info)
{
  if(info == NULL) {
    return -1;
  }
  
  if(wifi_status != WIFI_STATUS_CONNECTED) {
    return -1;
  }
  
  memcpy(info, &current_wifi_info, sizeof(wifi_info_t));
  return 0;
}
/*---------------------------------------------------------------------------*/
int8_t
wifi_manager_get_rssi(void)
{
  wifi_ap_record_t ap_info;
  
  if(wifi_status != WIFI_STATUS_CONNECTED) {
    return 0;
  }
  
  if(esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
    current_wifi_info.rssi = ap_info.rssi;
    return ap_info.rssi;
  }
  
  return 0;
}
/*---------------------------------------------------------------------------*/
