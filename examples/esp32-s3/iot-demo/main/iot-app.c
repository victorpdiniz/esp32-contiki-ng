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
 *         IoT Demo Application - Contiki-NG on ESP32-S3
 * \author
 *         Contiki-NG ESP32-S3 Port
 * 
 * \brief Demonstrates multi-process IoT architecture with:
 *        - LED indication (system heartbeat)
 *        - WiFi connectivity (via platform wifi-manager)
 *        - Sensor data collection (via platform sensor-manager)
 *        - Network communication patterns
 * 
 * This example shows pure Contiki-NG programming style with NO direct
 * hardware access. All ESP32-S3 specifics are abstracted in platform layer.
 */

#include "contiki.h"
#include "sys/etimer.h"
#include "sys/clock.h"
#include "dev/leds.h"

#include <stdio.h>
#include <string.h>

/* Platform hardware abstraction modules */
#include "wifi-manager.h"
#include "sensor-manager.h"

/* WiFi configuration - modify for your network */
#define WIFI_SSID     "CINGUESTS"
#define WIFI_PASSWORD "acessocin"

/*---------------------------------------------------------------------------*/
/* Process declarations */
PROCESS(led_blink_process, "LED Blink");
PROCESS(data_collector_process, "Data Collector");
PROCESS(network_process, "Network Manager");

AUTOSTART_PROCESSES(&led_blink_process, &data_collector_process, &network_process);

/* Export for platform.c */
extern struct process * const autostart_processes[];

/*---------------------------------------------------------------------------*/
/**
 * \brief LED Blink Process
 * 
 * Provides visual feedback that system is running.
 * Uses Contiki LED API - hardware details hidden in platform layer.
 */
PROCESS_THREAD(led_blink_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  printf("LED Blink Process started\n");

  /* Blink LED every 1 second */
  etimer_set(&timer, CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

    /* Toggle LED using Contiki API - use bitmask 1 for single LED */
    leds_toggle(1);

    etimer_reset(&timer);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/**
 * \brief Data Collector Process
 * 
 * Periodically collects sensor data and processes it.
 * Demonstrates event-driven programming and inter-process communication.
 */
PROCESS_THREAD(data_collector_process, ev, data)
{
  static struct etimer timer;
  static uint32_t reading_count = 0;

  PROCESS_BEGIN();

  printf("Data Collector Process started\n");

  /* Wait for sensor manager to initialize */
  etimer_set(&timer, CLOCK_SECOND * 2);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

  /* Set periodic timer - collect data every 10 seconds */
  etimer_set(&timer, CLOCK_SECOND * 10);

  while(1) {
    PROCESS_WAIT_EVENT();

    /* Handle sensor data events from platform layer */
    if(ev == sensor_event_data_ready) {
      sensor_data_t *sensor_data = (sensor_data_t *)data;
      if(sensor_data != NULL) {
        printf("Sensor Data Received:\n");
        printf("  Type: %d\n", sensor_data->type);
        printf("  Value: %.2f\n", sensor_data->value);
        printf("  Timestamp: %lu\n", sensor_data->timestamp);
        
        /* Process sensor data - examples:
         * - Store in flash using storage API
         * - Send via network (CoAP, MQTT, etc.)
         * - Trigger actuators based on thresholds
         * - Log to cloud services
         */
      }
    }
    
    /* Periodic data collection */
    if(ev == PROCESS_EVENT_TIMER && etimer_expired(&timer)) {
      reading_count++;
      printf("\n=== Reading cycle #%lu ===\n", reading_count);
      
      /* Read current sensor values */
      float temp, humidity;
      
      if(sensor_manager_read(SENSOR_TYPE_TEMPERATURE, &temp) == 0) {
        printf("Temperature: %.2f °C\n", temp);
      }
      
      if(sensor_manager_read(SENSOR_TYPE_HUMIDITY, &humidity) == 0) {
        printf("Humidity: %.2f %%\n", humidity);
      }
      
      etimer_reset(&timer);
    }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/**
 * \brief Network Process
 * 
 * Manages WiFi connectivity and network communication.
 * Handles connection events and demonstrates network operations.
 */
PROCESS_THREAD(network_process, ev, data)
{
  static struct etimer retry_timer;
  static bool wifi_connected = false;

  PROCESS_BEGIN();

  printf("Network Process started\n");

  /* Initial delay before connecting */
  etimer_set(&retry_timer, CLOCK_SECOND * 3);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&retry_timer));

  /* Initiate WiFi connection using platform abstraction */
  printf("Connecting to WiFi: %s\n", WIFI_SSID);
  wifi_manager_connect(WIFI_SSID, WIFI_PASSWORD);

  while(1) {
    PROCESS_WAIT_EVENT();

    /* Handle WiFi connection events from platform layer */
    if(ev == wifi_event_connected) {
      wifi_connected = true;
      printf("WiFi Connected!\n");
      printf("IP Address: %s\n", (char *)data);
      
      /* Now we can:
       * - Start CoAP server
       * - Connect to MQTT broker
       * - Sync time with NTP
       * - Register with cloud service
       */
    }
    else if(ev == wifi_event_disconnected) {
      wifi_connected = false;
      printf("WiFi Disconnected!\n");
      
      /* Retry connection after delay */
      etimer_set(&retry_timer, CLOCK_SECOND * 10);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&retry_timer));
      
      printf("Retrying WiFi connection...\n");
      wifi_manager_connect(WIFI_SSID, WIFI_PASSWORD);
    }
    
    /* Could add periodic tasks when connected:
     * - Send telemetry data
     * - Check for firmware updates
     * - Ping server to keep connection alive
     */
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
