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
 *         Contiki configuration for ESP32-S3 platform
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_

#include <stdint.h>
#include <inttypes.h>

/*---------------------------------------------------------------------------*/
/* Include platform-specific configuration */
#include "esp32-s3-conf.h"

/*---------------------------------------------------------------------------*/
/* Compiler configuration */
#define CCIF
#define CLIF

/*---------------------------------------------------------------------------*/
/* Log configuration - Use ESP-IDF logging, avoid conflict with ESP-IDF */
#ifndef __ASSEMBLER__
/* ESP-IDF includes must come after all defines to avoid LOG_LEVEL conflict */
#endif

#ifndef LOG_CONF_LEVEL_MAIN
#define LOG_CONF_LEVEL_MAIN  3  /* INFO level */
#endif

/*---------------------------------------------------------------------------*/
/* Clock and timer configuration */
#ifndef CLOCK_CONF_SECOND
#define CLOCK_CONF_SECOND                   128   /* Contiki default ticks/sec */
#endif

/* Rtimer configuration */
#define RTIMER_CONF_CLOCK_SIZE              4
typedef uint32_t rtimer_clock_t;
#define RTIMER_CLOCK_DIFF(a, b)             ((int32_t)((a) - (b)))

/*---------------------------------------------------------------------------*/
/* LED configuration - Use legacy API with bitmask */
#define LEDS_CONF_LEGACY_API                1
#define LEDS_CONF_LED1                      1  /* Bitmask: bit 0 */
#define LEDS_CONF_ALL                       1  /* Single LED board */

/*---------------------------------------------------------------------------*/
/* Network configuration - Minimal for now */
#define NETSTACK_CONF_NETWORK               nullnet_driver

/*---------------------------------------------------------------------------*//* Application overrides - include project-specific configuration last */
#ifdef PROJECT_CONF_H
#include PROJECT_CONF_H
#endif

/*---------------------------------------------------------------------------*/#endif /* CONTIKI_CONF_H_ */

/**
 * @}
 */
