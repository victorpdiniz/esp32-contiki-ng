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
 *         Sensor Manager Implementation
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#include "sensor-manager.h"
#include "freertos-bridge.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "Sensor-Manager";

/* Sensor data storage */
static sensor_data_t last_readings[5];
static SemaphoreHandle_t sensor_mutex = NULL;

/* Process event */
process_event_t sensor_event_data_ready;

/* Process to handle sensor events */
PROCESS(sensor_event_process, "Sensor Event Process");

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_event_process, ev, data)
{
  PROCESS_BEGIN();
  
  while(1) {
    PROCESS_WAIT_EVENT();
    
    if(ev == sensor_event_data_ready) {
      sensor_data_t *sensor_data = (sensor_data_t *)data;
      if(sensor_data != NULL) {
        ESP_LOGI(TAG, "Contiki received sensor data: type=%d, value=%.2f",
                 sensor_data->type, sensor_data->value);
        free(data);
      }
    }
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Simulated sensor reading (replace with real sensor driver)
 */
static float
read_sensor_hw(sensor_type_t type)
{
  /* This is a simulation - replace with actual sensor hardware code */
  switch(type) {
    case SENSOR_TYPE_TEMPERATURE:
      /* Simulate temperature reading (20-30°C) */
      return 20.0f + (float)(esp_random() % 100) / 10.0f;
      
    case SENSOR_TYPE_HUMIDITY:
      /* Simulate humidity reading (40-80%) */
      return 40.0f + (float)(esp_random() % 400) / 10.0f;
      
    case SENSOR_TYPE_PRESSURE:
      /* Simulate pressure reading (980-1020 hPa) */
      return 980.0f + (float)(esp_random() % 400) / 10.0f;
      
    case SENSOR_TYPE_LIGHT:
      /* Simulate light reading (0-1000 lux) */
      return (float)(esp_random() % 1000);
      
    case SENSOR_TYPE_MOTION:
      /* Simulate motion detection (0 or 1) */
      return (float)(esp_random() % 2);
      
    default:
      return 0.0f;
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief FreeRTOS task for periodic sensor reading
 */
static void
sensor_task(void *pvParameters)
{
  ESP_LOGI(TAG, "Sensor task started");
  
  TickType_t last_wake_time = xTaskGetTickCount();
  const TickType_t frequency = pdMS_TO_TICKS(5000); /* Read every 5 seconds */
  
  while(1) {
    /* Read temperature sensor */
    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    if(data != NULL) {
      data->type = SENSOR_TYPE_TEMPERATURE;
      data->value = read_sensor_hw(SENSOR_TYPE_TEMPERATURE);
      data->timestamp = xTaskGetTickCount();
      
      /* Store in local cache with mutex protection */
      if(freertos_take_mutex(sensor_mutex, 100) == pdTRUE) {
        last_readings[SENSOR_TYPE_TEMPERATURE] = *data;
        freertos_give_mutex(sensor_mutex);
      }
      
      ESP_LOGI(TAG, "Temperature: %.2f °C", data->value);
      
      /* Post event to Contiki process */
      freertos_post_to_contiki(&sensor_event_process,
                              sensor_event_data_ready,
                              data, sizeof(sensor_data_t));
    }
    
    /* Read humidity sensor */
    data = malloc(sizeof(sensor_data_t));
    if(data != NULL) {
      data->type = SENSOR_TYPE_HUMIDITY;
      data->value = read_sensor_hw(SENSOR_TYPE_HUMIDITY);
      data->timestamp = xTaskGetTickCount();
      
      if(freertos_take_mutex(sensor_mutex, 100) == pdTRUE) {
        last_readings[SENSOR_TYPE_HUMIDITY] = *data;
        freertos_give_mutex(sensor_mutex);
      }
      
      ESP_LOGI(TAG, "Humidity: %.2f %%", data->value);
      
      freertos_post_to_contiki(&sensor_event_process,
                              sensor_event_data_ready,
                              data, sizeof(sensor_data_t));
    }
    
    /* Wait for next reading cycle */
    vTaskDelayUntil(&last_wake_time, frequency);
  }
}
/*---------------------------------------------------------------------------*/
void
sensor_manager_init(void)
{
  /* Allocate event */
  sensor_event_data_ready = process_alloc_event();
  
  /* Start sensor event process */
  process_start(&sensor_event_process, NULL);
  
  /* Create mutex for sensor data access */
  sensor_mutex = freertos_create_mutex();
  
  /* Initialize sensor data */
  memset(last_readings, 0, sizeof(last_readings));
  
  /* Create FreeRTOS task for sensor reading */
  xTaskCreate(sensor_task,
              "sensor_task",
              4096,
              NULL,
              3,
              NULL);
  
  ESP_LOGI(TAG, "Sensor Manager initialized");
}
/*---------------------------------------------------------------------------*/
int
sensor_manager_read(sensor_type_t type, float *value)
{
  if(value == NULL || type >= 5) {
    return -1;
  }
  
  if(freertos_take_mutex(sensor_mutex, 100) == pdTRUE) {
    *value = last_readings[type].value;
    freertos_give_mutex(sensor_mutex);
    return 0;
  }
  
  return -1;
}
/*---------------------------------------------------------------------------*/
int
sensor_manager_get_last_reading(sensor_type_t type, sensor_data_t *data)
{
  if(data == NULL || type >= 5) {
    return -1;
  }
  
  if(freertos_take_mutex(sensor_mutex, 100) == pdTRUE) {
    memcpy(data, &last_readings[type], sizeof(sensor_data_t));
    freertos_give_mutex(sensor_mutex);
    return 0;
  }
  
  return -1;
}
/*---------------------------------------------------------------------------*/
