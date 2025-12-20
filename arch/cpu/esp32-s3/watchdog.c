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
 * \file
 *         Watchdog implementation for ESP32-S3 using ESP-IDF Task WDT
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#include "contiki.h"
#include "dev/watchdog.h"

/* ESP-IDF includes */
#include "esp_task_wdt.h"
#include "esp_system.h"
#include "esp_log.h"

/*---------------------------------------------------------------------------*/
void
watchdog_init(void)
{
  /* Subscribe current task to Task WDT for monitoring */
  esp_err_t ret = esp_task_wdt_add(NULL);
  if(ret == ESP_ERR_INVALID_ARG) {
    /* Task already added, ignore */
  } else if(ret != ESP_OK) {
    ESP_LOGW("watchdog", "Failed to add task to WDT: %s", esp_err_to_name(ret));
  }
}
/*---------------------------------------------------------------------------*/
void
watchdog_start(void)
{
  /* Task WDT is already running in ESP-IDF */
}
/*---------------------------------------------------------------------------*/
void
watchdog_periodic(void)
{
  /* Reset the watchdog timer for this task */
  esp_err_t ret = esp_task_wdt_reset();
  if(ret != ESP_OK && ret != ESP_ERR_INVALID_ARG) {
    /* Log only if it's not an "already reset" or "task not subscribed" error */
    if(ret != ESP_ERR_NOT_FOUND) {
      ESP_LOGW("watchdog", "Failed to reset WDT: %s", esp_err_to_name(ret));
    }
  }
}
/*---------------------------------------------------------------------------*/
void
watchdog_stop(void)
{
  /* Unsubscribe task from watchdog */
  esp_task_wdt_delete(NULL);
}
/*---------------------------------------------------------------------------*/
void
watchdog_reboot(void)
{
  /* Trigger system restart */
  esp_restart();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 */
