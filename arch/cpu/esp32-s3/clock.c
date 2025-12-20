/*
 * Copyright (c) 2025, Victor
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*---------------------------------------------------------------------------*/
/* Boot time in microseconds */
static uint64_t boot_us;

/*---------------------------------------------------------------------------*/
void
clock_init(void)
{
  boot_us = esp_timer_get_time();
}
/*---------------------------------------------------------------------------*/
clock_time_t
clock_time(void)
{
  /* Return ticks since boot, scaled by CLOCK_CONF_SECOND */
  /* esp_timer_get_time() returns microseconds */
  uint64_t us_since_boot = esp_timer_get_time() - boot_us;
  return (clock_time_t)((us_since_boot * CLOCK_CONF_SECOND) / 1000000ULL);
}
/*---------------------------------------------------------------------------*/
unsigned long
clock_seconds(void)
{
  /* Return seconds since boot */
  return (unsigned long)((esp_timer_get_time() - boot_us) / 1000000ULL);
}
/*---------------------------------------------------------------------------*/
void
clock_wait(clock_time_t t)
{
  clock_time_t start = clock_time();
  while(clock_time() - start < t) {
    /* Yield to FreeRTOS */
    vTaskDelay(1);
  }
}
/*---------------------------------------------------------------------------*/
void
clock_delay_usec(uint16_t dt)
{
  /* Busy-wait for precise microsecond delays */
  uint64_t target = esp_timer_get_time() + dt;
  while(esp_timer_get_time() < target) {
    /* Busy loop */
  }
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
