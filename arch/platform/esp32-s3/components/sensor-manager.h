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
 *         ESP32-S3 Sensor Manager Header
 * \author
 *         Contiki-NG ESP32-S3 Port
 * 
 * \brief Sensor reading using FreeRTOS task with Contiki integration
 */

#ifndef SENSOR_MANAGER_H_
#define SENSOR_MANAGER_H_

#include "contiki.h"
#include <stdint.h>

/*---------------------------------------------------------------------------*/
/* Sensor types */
typedef enum {
  SENSOR_TYPE_TEMPERATURE,
  SENSOR_TYPE_HUMIDITY,
  SENSOR_TYPE_PRESSURE,
  SENSOR_TYPE_LIGHT,
  SENSOR_TYPE_MOTION
} sensor_type_t;

/*---------------------------------------------------------------------------*/
/* Sensor data structure */
typedef struct {
  sensor_type_t type;
  float value;
  uint32_t timestamp;
} sensor_data_t;

/*---------------------------------------------------------------------------*/
/* Process events */
extern process_event_t sensor_event_data_ready;

/*---------------------------------------------------------------------------*/
/**
 * \brief Initialize sensor manager
 * 
 * Creates FreeRTOS task for sensor reading
 */
void sensor_manager_init(void);

/**
 * \brief Read sensor value
 * 
 * \param type Sensor type to read
 * \param value Pointer to store the value
 * 
 * \return 0 on success, -1 on failure
 */
int sensor_manager_read(sensor_type_t type, float *value);

/**
 * \brief Get last sensor reading
 * 
 * \param type Sensor type
 * \param data Pointer to sensor_data_t structure
 * 
 * \return 0 on success, -1 on failure
 */
int sensor_manager_get_last_reading(sensor_type_t type, sensor_data_t *data);

/*---------------------------------------------------------------------------*/
#endif /* SENSOR_MANAGER_H_ */
