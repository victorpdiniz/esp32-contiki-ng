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
 * \addtogroup esp32-s3-platform
 * @{
 *
 * \file
 *         ESP32-S3 platform - Contiki-NG running as FreeRTOS task
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#include "contiki.h"
#include "sys/platform.h"
#include "sys/node-id.h"
#include "sys/rtimer.h"
#include "sys/autostart.h"
#include "dev/watchdog.h"

#include <stdio.h>
#include <stdint.h>

/* ESP-IDF includes */
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "Contiki-NG";

#define CONTIKI_TASK_STACK_SIZE 8192
#define CONTIKI_TASK_PRIORITY   5

/* External autostart array from main application */
extern struct process * const autostart_processes[];
/*---------------------------------------------------------------------------*/
void
platform_init_stage_one(void)
{
  esp_err_t ret;

  /* Initialize NVS */
  ret = nvs_flash_init();
  if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    ret = nvs_flash_init();
  }

  ESP_LOGI(TAG, "Platform Init Stage One");
}
/*---------------------------------------------------------------------------*/
void
platform_init_stage_two(void)
{
  esp_chip_info_t chip_info;
  uint32_t flash_size;

  esp_chip_info(&chip_info);
  esp_flash_get_size(NULL, &flash_size);

  ESP_LOGI(TAG, "ESP32-S3 with %d CPU cores, WiFi%s%s",
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
  ESP_LOGI(TAG, "Silicon revision %d, %luMB %s flash",
           chip_info.revision,
           flash_size / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}
/*---------------------------------------------------------------------------*/
void
platform_init_stage_three(void)
{
  ESP_LOGI(TAG, "Platform Init Stage Three");
}
/*---------------------------------------------------------------------------*/
void
platform_idle(void)
{
  /* Yield to FreeRTOS scheduler */
  vTaskDelay(1);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Main Contiki-NG task
 */
static void
contiki_ng_task(void *pvParameters)
{
  ESP_LOGI(TAG, "Starting Contiki-NG task...");

  /* Platform initialization stage one */
  platform_init_stage_one();

  /* Initialize Contiki-NG */
  clock_init();
  rtimer_init();
  process_init();
  process_start(&etimer_process, NULL);
  ctimer_init();
  watchdog_init();

  /* Platform initialization stage two */
  platform_init_stage_two();

  /* Start autostart processes */
  autostart_start(autostart_processes);

  /* Platform initialization stage three */
  platform_init_stage_three();

  ESP_LOGI(TAG, "Entering main loop");

  /* Main loop */
  while(1) {
    uint8_t r;
    do {
      r = process_run();
      watchdog_periodic();
    } while(r > 0);

    /* Idle - yield to other tasks */
    platform_idle();
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Application main entry point (called by ESP-IDF)
 */
void
app_main(void)
{
  ESP_LOGI(TAG, "ESP32-S3 Contiki-NG Port");

  /* Create Contiki-NG task */
  xTaskCreate(contiki_ng_task,
              "contiki_ng",
              CONTIKI_TASK_STACK_SIZE,
              NULL,
              CONTIKI_TASK_PRIORITY,
              NULL);
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 */
