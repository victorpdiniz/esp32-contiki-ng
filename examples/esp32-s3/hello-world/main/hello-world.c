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
 *         Hello World example for ESP32-S3
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#include "contiki.h"
#include "sys/etimer.h"
#include "sys/clock.h"

#include <stdio.h>

/* ESP-IDF includes for logging and GPIO */
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "Hello-World";

/* LED GPIO pin - Heltec WiFi LoRa 32 V3 has LED on GPIO 35 */
#define LED_GPIO  GPIO_NUM_35

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello World process");
AUTOSTART_PROCESSES(&hello_world_process);

/* Export autostart_processes for platform.c */
extern struct process * const autostart_processes[];
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  static struct etimer timer;
  static uint8_t led_state = 0;

  PROCESS_BEGIN();

  ESP_LOGI(TAG, "Hello, World from ESP32-S3!");
  ESP_LOGI(TAG, "Contiki-NG is running successfully!");
  
  /* Reset GPIO to default state first */
  gpio_reset_pin(LED_GPIO);
  
  /* Configure LED GPIO as output */
  gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = (1ULL << LED_GPIO),
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE
  };
  
  esp_err_t ret = gpio_config(&io_conf);
  if (ret == ESP_OK) {
    ESP_LOGI(TAG, "LED configured on GPIO %d - SUCCESS", LED_GPIO);
    gpio_set_level(LED_GPIO, 0);
    ESP_LOGI(TAG, "LED initial state: OFF");
  } else {
    ESP_LOGE(TAG, "Failed to configure GPIO %d, error: %d", LED_GPIO, ret);
  }

  /* Set a periodic timer that expires every second */
  etimer_set(&timer, CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

    /* Toggle LED state */
    led_state = !led_state;
    gpio_set_level(LED_GPIO, led_state);

    ESP_LOGI(TAG, "Tick! Uptime: %lu seconds, LED: %s", 
             clock_seconds(), led_state ? "ON" : "OFF");
    
    /* Reset the timer */
    etimer_reset(&timer);
  }  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
