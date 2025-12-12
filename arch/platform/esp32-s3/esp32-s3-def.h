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
 *         Platform-specific definitions for ESP32-S3
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#ifndef ESP32_S3_DEF_H_
#define ESP32_S3_DEF_H_

#include <stdint.h>

/*---------------------------------------------------------------------------*/
/* Platform name */
#define PLATFORM_NAME "esp32-s3"

/*---------------------------------------------------------------------------*/
/* Clock configuration */
#ifndef CLOCK_CONF_SECOND
#define CLOCK_CONF_SECOND 128
#endif

/*---------------------------------------------------------------------------*/
/* UART configuration for console */
#ifndef CONFIG_ESP_CONSOLE_UART_NUM
#define CONFIG_ESP_CONSOLE_UART_NUM 0
#endif

#ifndef CONFIG_ESP_CONSOLE_UART_BAUDRATE
#define CONFIG_ESP_CONSOLE_UART_BAUDRATE 115200
#endif

/*---------------------------------------------------------------------------*/
/* Memory configuration */
#define PLATFORM_HAS_LEDS    0
#define PLATFORM_HAS_BUTTON  0
#define PLATFORM_HAS_RADIO   0

/*---------------------------------------------------------------------------*/
/* Types - Use standard Contiki-NG types */
typedef uint32_t uip_stats_t;

/*---------------------------------------------------------------------------*/
/* Watchdog configuration */
#define WATCHDOG_CONF_ENABLE 1

/*---------------------------------------------------------------------------*/
#endif /* ESP32_S3_DEF_H_ */

/**
 * @}
 */
