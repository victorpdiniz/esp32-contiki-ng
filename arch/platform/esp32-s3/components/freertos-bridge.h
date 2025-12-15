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
 *         FreeRTOS-Contiki Bridge - Inter-task communication
 * \author
 *         Contiki-NG ESP32-S3 Port
 * 
 * \brief Bridge layer between FreeRTOS tasks and Contiki-NG processes
 * 
 * This module provides:
 * - Event posting from FreeRTOS tasks to Contiki processes
 * - Safe mutex/semaphore wrappers for Contiki processes
 * - Queue-based message passing
 */

#ifndef FREERTOS_BRIDGE_H_
#define FREERTOS_BRIDGE_H_

#include "contiki.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

/*---------------------------------------------------------------------------*/
/* Event types for FreeRTOS -> Contiki communication */
#define FREERTOS_EVENT_WIFI_CONNECTED    0x01
#define FREERTOS_EVENT_WIFI_DISCONNECTED 0x02
#define FREERTOS_EVENT_SENSOR_DATA       0x03
#define FREERTOS_EVENT_BUTTON_PRESS      0x04
#define FREERTOS_EVENT_CUSTOM            0x80

/*---------------------------------------------------------------------------*/
/* Message structure for queue-based communication */
typedef struct {
  uint8_t event_type;
  void *data;
  size_t data_len;
} freertos_message_t;

/*---------------------------------------------------------------------------*/
/* Bridge configuration */
#define BRIDGE_QUEUE_LENGTH 16
#define BRIDGE_TASK_PRIORITY 4
#define BRIDGE_TASK_STACK_SIZE 2048

/*---------------------------------------------------------------------------*/
/**
 * \brief Initialize the FreeRTOS-Contiki bridge
 * 
 * Must be called before using any bridge functions
 */
void freertos_bridge_init(void);

/**
 * \brief Post event from FreeRTOS task to Contiki process
 * 
 * \param process Pointer to the target Contiki process
 * \param event Event type to post
 * \param data Event data (will be copied)
 * \param data_len Length of event data
 * 
 * \return 0 on success, -1 on failure
 * 
 * Thread-safe: Can be called from any FreeRTOS task
 */
int freertos_post_to_contiki(struct process *process, 
                              process_event_t event,
                              const void *data,
                              size_t data_len);

/**
 * \brief Create a mutex that can be used from Contiki processes
 * 
 * \return Mutex handle or NULL on failure
 */
SemaphoreHandle_t freertos_create_mutex(void);

/**
 * \brief Take a mutex (blocking)
 * 
 * \param mutex Mutex handle
 * \param timeout_ms Timeout in milliseconds (use portMAX_DELAY for infinite)
 * 
 * \return pdTRUE if mutex acquired, pdFALSE on timeout
 */
BaseType_t freertos_take_mutex(SemaphoreHandle_t mutex, uint32_t timeout_ms);

/**
 * \brief Give a mutex
 * 
 * \param mutex Mutex handle
 */
void freertos_give_mutex(SemaphoreHandle_t mutex);

/**
 * \brief Get current FreeRTOS tick count
 * 
 * \return Current tick count
 */
uint32_t freertos_get_ticks(void);

/**
 * \brief Get FreeRTOS tick rate (ticks per second)
 * 
 * \return Tick rate in Hz
 */
uint32_t freertos_get_tick_rate(void);

/**
 * \brief Process pending bridge events
 * 
 * Must be called from Contiki main loop to process events
 * posted from FreeRTOS tasks
 */
void freertos_bridge_process_events(void);

/*---------------------------------------------------------------------------*/
#endif /* FREERTOS_BRIDGE_H_ */
