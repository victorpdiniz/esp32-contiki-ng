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
 *         LED driver implementation for ESP32-S3
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#include "dev/leds-arch.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "leds-arch";

/* Platform-specific GPIO mapping */
#ifndef PLATFORM_CONF_LED1_PIN
#define LED1_GPIO  35  /* Heltec WiFi LoRa 32 V3 default */
#else
#define LED1_GPIO  PLATFORM_CONF_LED1_PIN
#endif

/* Some boards use inverted logic (LED on when GPIO LOW) */
#ifndef PLATFORM_CONF_LED_INVERTED
#define LED_INVERTED  1  /* Heltec boards typically use inverted logic */
#else
#define LED_INVERTED  PLATFORM_CONF_LED_INVERTED
#endif

static unsigned char leds_state = 0;

/*---------------------------------------------------------------------------*/
void
leds_arch_init(void)
{
  gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = (1ULL << LED1_GPIO),
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE
  };
  
  gpio_config(&io_conf);
  gpio_set_level(LED1_GPIO, LED_INVERTED ? 1 : 0);
  leds_state = 0;
  
  ESP_LOGI(TAG, "LED driver initialized (GPIO %d)", LED1_GPIO);
}
/*---------------------------------------------------------------------------*/
unsigned char
leds_arch_get(void)
{
  return leds_state;
}
/*---------------------------------------------------------------------------*/
void
leds_arch_set(unsigned char leds)
{
  leds_state = leds;
  
  /* Set GPIO level based on LED state (inverted logic for Heltec boards) */
  if(leds & 1) {
    gpio_set_level(LED1_GPIO, LED_INVERTED ? 0 : 1);
  } else {
    gpio_set_level(LED1_GPIO, LED_INVERTED ? 1 : 0);
  }
}
/*---------------------------------------------------------------------------*/
