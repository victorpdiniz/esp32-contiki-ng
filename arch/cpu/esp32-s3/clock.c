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
 * \addtogroup esp32-s3
 * @{
 *
 * \defgroup esp32-s3-clock ESP32-S3 clock
 *
 * Implementation of the clock module for ESP32-S3 using ESP-IDF timers
 *
 * @{
 *
 * \file
 *         Implementation of the clock module for ESP32-S3
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#include "contiki.h"
#include "sys/clock.h"
#include "sys/etimer.h"

/* ESP-IDF includes */
#include "esp_timer.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*---------------------------------------------------------------------------*/
/* Number of ticks per second - Contiki-NG default is 128 */
#ifndef CLOCK_CONF_SECOND
#define CLOCK_CONF_SECOND 128
#endif

/*---------------------------------------------------------------------------*/
static volatile clock_time_t current_clock = 0;
static volatile unsigned long current_seconds = 0;
static unsigned int second_countdown = CLOCK_SECOND;
static esp_timer_handle_t clock_timer_handle;

/*---------------------------------------------------------------------------*/
/**
 * \brief Timer callback for system clock
 */
static void
clock_timer_callback(void *arg)
{
  current_clock++;

  if(etimer_pending()) {
    etimer_request_poll();
  }

  if(--second_countdown == 0) {
    current_seconds++;
    second_countdown = CLOCK_SECOND;
  }
}
/*---------------------------------------------------------------------------*/
void
clock_init(void)
{
  const esp_timer_create_args_t timer_args = {
    .callback = &clock_timer_callback,
    .arg = NULL,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "contiki_clock"
  };

  current_clock = 0;
  current_seconds = 0;
  second_countdown = CLOCK_SECOND;

  /* Create and start periodic timer */
  esp_timer_create(&timer_args, &clock_timer_handle);
  esp_timer_start_periodic(clock_timer_handle, 1000000 / CLOCK_SECOND);
}
/*---------------------------------------------------------------------------*/
clock_time_t
clock_time(void)
{
  return current_clock;
}
/*---------------------------------------------------------------------------*/
unsigned long
clock_seconds(void)
{
  return current_seconds;
}
/*---------------------------------------------------------------------------*/
void
clock_wait(clock_time_t t)
{
  clock_time_t start;
  start = clock_time();
  while(clock_time() - start < t) {
    vTaskDelay(1);
  }
}
/*---------------------------------------------------------------------------*/
void
clock_delay_usec(uint16_t dt)
{
  esp_rom_delay_us(dt);
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
