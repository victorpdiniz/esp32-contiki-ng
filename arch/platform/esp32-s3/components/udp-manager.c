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
 *         UDP Manager Implementation
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#include "udp-manager.h"
#include "freertos-bridge.h"
#include "sys/log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define LOG_MODULE "UDP-Manager"
#define LOG_LEVEL LOG_LEVEL_INFO

static const char *TAG = "UDP-Manager";

/* Maximum number of UDP connections */
#define MAX_UDP_CONNECTIONS 8

/* UDP connection pool */
static udp_connection_t connection_pool[MAX_UDP_CONNECTIONS];
static int active_connections = 0;

/* Task handle for UDP receiver task */
static TaskHandle_t udp_task_handle = NULL;
static int udp_monitor_running = 0;

/*---------------------------------------------------------------------------*/
/* UDP monitor task - runs in FreeRTOS to check for incoming data */
static void
udp_monitor_task(void *arg)
{
  fd_set readset;
  struct timeval timeout;
  int max_fd;
  int i;
  
  ESP_LOGI(TAG, "UDP monitor task started");
  
  while(udp_monitor_running) {
    FD_ZERO(&readset);
    max_fd = -1;
    
    /* Add all active sockets to read set */
    for(i = 0; i < MAX_UDP_CONNECTIONS; i++) {
      if(connection_pool[i].sock > 0) {
        FD_SET(connection_pool[i].sock, &readset);
        if(connection_pool[i].sock > max_fd) {
          max_fd = connection_pool[i].sock;
        }
      }
    }
    
    if(max_fd < 0) {
      /* No active connections, sleep briefly */
      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }
    
    /* Wait for data on any socket */
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000; /* 100ms */
    
    int ret = select(max_fd + 1, &readset, NULL, NULL, &timeout);
    
    if(ret > 0) {
      /* Check each connection for data */
      for(i = 0; i < MAX_UDP_CONNECTIONS; i++) {
        if(connection_pool[i].sock > 0 && FD_ISSET(connection_pool[i].sock, &readset)) {
          /* Data available on this socket */
          uint8_t buffer[1024];
          struct sockaddr_in remote_addr;
          socklen_t addr_len = sizeof(remote_addr);
          
          int len = recvfrom(connection_pool[i].sock, buffer, sizeof(buffer), 0,
                            (struct sockaddr *)&remote_addr, &addr_len);
          
          if(len > 0 && connection_pool[i].callback) {
            /* Call the registered callback */
            connection_pool[i].callback(&connection_pool[i],
                                       (uint32_t *)&remote_addr.sin_addr.s_addr,
                                       ntohs(remote_addr.sin_port),
                                       buffer,
                                       len);
          }
        }
      }
    }
  }
  
  ESP_LOGI(TAG, "UDP monitor task ended");
  vTaskDelete(NULL);
}

/*---------------------------------------------------------------------------*/
void
udp_manager_init(void)
{
  int i;
  
  /* Initialize connection pool */
  for(i = 0; i < MAX_UDP_CONNECTIONS; i++) {
    connection_pool[i].sock = -1;
    connection_pool[i].local_port = 0;
    connection_pool[i].remote_port = 0;
    connection_pool[i].callback = NULL;
    connection_pool[i].callback_process = NULL;
  }
  
  active_connections = 0;
  
  /* Start UDP monitor task */
  udp_monitor_running = 1;
  BaseType_t ret = xTaskCreate(udp_monitor_task, "udp_monitor", 4096, NULL, 10, &udp_task_handle);
  
  if(ret != pdPASS) {
    ESP_LOGE(TAG, "Failed to create UDP monitor task");
    udp_monitor_running = 0;
  } else {
    LOG_INFO("UDP Manager initialized\n");
  }
}

/*---------------------------------------------------------------------------*/
int
udp_register(udp_connection_t *c,
             uint16_t local_port,
             const uint32_t *remote_addr,
             uint16_t remote_port,
             void (*callback)(struct udp_connection *c,
                              const uint32_t *remote_addr,
                              uint16_t remote_port,
                              const uint8_t *data,
                              uint16_t datalen))
{
  struct sockaddr_in addr;
  int sock;
  
  if(c == NULL) {
    LOG_ERR("Invalid parameters\n");
    return -1;
  }
  
  /* Callback is optional - only needed for receiving */
  
  if(active_connections >= MAX_UDP_CONNECTIONS) {
    LOG_ERR("Max connections reached\n");
    return -1;
  }
  
  /* Create UDP socket */
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sock < 0) {
    LOG_ERR("Failed to create socket\n");
    return -1;
  }
  
  /* Set socket to non-blocking */
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);
  
  /* Bind to local port */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(local_port);
  
  if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    LOG_ERR("Failed to bind socket to port %u\n", local_port);
    close(sock);
    return -1;
  }
  
  /* Initialize connection structure */
  c->sock = sock;
  c->local_port = local_port;
  c->callback = callback;
  c->callback_process = NULL;
  
  if(remote_addr != NULL) {
    c->remote_port = remote_port;
  } else {
    c->remote_port = 0;
  }
  
  /* Add to connection pool for monitoring */
  int slot = -1;
  for(int i = 0; i < MAX_UDP_CONNECTIONS; i++) {
    if(connection_pool[i].sock < 0) {
      slot = i;
      break;
    }
  }
  
  if(slot >= 0) {
    memcpy(&connection_pool[slot], c, sizeof(udp_connection_t));
  }
  
  active_connections++;
  
  LOG_INFO("UDP connection registered on port %u (slot %d)\n", local_port, slot);
  
  return 0;
}

/*---------------------------------------------------------------------------*/
void
udp_unregister(udp_connection_t *c)
{
  if(c == NULL || c->sock < 0) {
    return;
  }
  
  close(c->sock);
  c->sock = -1;
  c->callback = NULL;
  active_connections--;
  
  LOG_INFO("UDP connection unregistered\n");
}

/*---------------------------------------------------------------------------*/
int
udp_send_data(udp_connection_t *c,
              const void *data,
              uint16_t datalen,
              const uint32_t *remote_addr,
              uint16_t remote_port)
{
  struct sockaddr_in addr;
  int ret;
  
  if(c == NULL || c->sock < 0 || data == NULL) {
    return -1;
  }
  
  /* Prepare remote address */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = *remote_addr;
  addr.sin_port = htons(remote_port);
  
  /* Send data */
  ret = sendto(c->sock, data, datalen, 0, (struct sockaddr *)&addr, sizeof(addr));
  
  if(ret < 0) {
    LOG_ERR("Failed to send %u bytes to UDP socket\n", datalen);
    return -1;
  }
  
  return ret;
}

/*---------------------------------------------------------------------------*/
uint32_t
udp_ip4addr_from_string(const char *addr)
{
  struct in_addr ip;
  
  if(addr == NULL || inet_pton(AF_INET, addr, &ip) <= 0) {
    LOG_ERR("Invalid IPv4 address: %s\n", addr ? addr : "NULL");
    return 0;
  }
  
  return ip.s_addr;
}

/*---------------------------------------------------------------------------*/
char *
udp_ip4addr_to_string(uint32_t addr, char *buf)
{
  struct in_addr ip;
  
  if(buf == NULL) {
    return NULL;
  }
  
  ip.s_addr = addr;
  
  if(inet_ntop(AF_INET, &ip, buf, INET_ADDRSTRLEN) == NULL) {
    snprintf(buf, INET_ADDRSTRLEN, "0.0.0.0");
  }
  
  return buf;
}

/*---------------------------------------------------------------------------*/
