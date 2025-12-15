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
 *         FreeRTOS-Contiki Bridge Implementation
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#include "freertos-bridge.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "FreeRTOS-Bridge";

/* Message queue for inter-task communication */
static QueueHandle_t bridge_queue = NULL;

/* Event buffer for storing data */
#define MAX_EVENT_DATA_SIZE 256

typedef struct {
  struct process *target_process;
  process_event_t event;
  uint8_t data[MAX_EVENT_DATA_SIZE];
  size_t data_len;
} bridge_event_t;

/*---------------------------------------------------------------------------*/
void
freertos_bridge_init(void)
{
  /* Create message queue */
  bridge_queue = xQueueCreate(BRIDGE_QUEUE_LENGTH, sizeof(bridge_event_t));
  
  if(bridge_queue == NULL) {
    ESP_LOGE(TAG, "Failed to create bridge queue");
    return;
  }
  
  ESP_LOGI(TAG, "FreeRTOS-Contiki bridge initialized");
}
/*---------------------------------------------------------------------------*/
int
freertos_post_to_contiki(struct process *process, 
                         process_event_t event,
                         const void *data,
                         size_t data_len)
{
  bridge_event_t bridge_event;
  
  if(bridge_queue == NULL) {
    ESP_LOGE(TAG, "Bridge not initialized");
    return -1;
  }
  
  if(data_len > MAX_EVENT_DATA_SIZE) {
    ESP_LOGE(TAG, "Event data too large: %d bytes", data_len);
    return -1;
  }
  
  /* Prepare bridge event */
  bridge_event.target_process = process;
  bridge_event.event = event;
  bridge_event.data_len = data_len;
  
  if(data != NULL && data_len > 0) {
    memcpy(bridge_event.data, data, data_len);
  }
  
  /* Post to queue */
  if(xQueueSend(bridge_queue, &bridge_event, 0) != pdTRUE) {
    ESP_LOGW(TAG, "Bridge queue full, event dropped");
    return -1;
  }
  
  return 0;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Process bridge events (called from Contiki main loop)
 */
void
freertos_bridge_process_events(void)
{
  bridge_event_t bridge_event;
  
  if(bridge_queue == NULL) {
    return;
  }
  
  /* Process all pending events */
  while(xQueueReceive(bridge_queue, &bridge_event, 0) == pdTRUE) {
    if(bridge_event.target_process != NULL) {
      /* Allocate memory for event data if needed */
      void *event_data = NULL;
      if(bridge_event.data_len > 0) {
        event_data = malloc(bridge_event.data_len);
        if(event_data != NULL) {
          memcpy(event_data, bridge_event.data, bridge_event.data_len);
        }
      }
      
      /* Post event to Contiki process */
      process_post(bridge_event.target_process, 
                   bridge_event.event, 
                   event_data);
    }
  }
}
/*---------------------------------------------------------------------------*/
SemaphoreHandle_t
freertos_create_mutex(void)
{
  return xSemaphoreCreateMutex();
}
/*---------------------------------------------------------------------------*/
BaseType_t
freertos_take_mutex(SemaphoreHandle_t mutex, uint32_t timeout_ms)
{
  TickType_t ticks;
  
  if(timeout_ms == portMAX_DELAY) {
    ticks = portMAX_DELAY;
  } else {
    ticks = pdMS_TO_TICKS(timeout_ms);
  }
  
  return xSemaphoreTake(mutex, ticks);
}
/*---------------------------------------------------------------------------*/
void
freertos_give_mutex(SemaphoreHandle_t mutex)
{
  xSemaphoreGive(mutex);
}
/*---------------------------------------------------------------------------*/
uint32_t
freertos_get_ticks(void)
{
  return xTaskGetTickCount();
}
/*---------------------------------------------------------------------------*/
uint32_t
freertos_get_tick_rate(void)
{
  return configTICK_RATE_HZ;
}
/*---------------------------------------------------------------------------*/
