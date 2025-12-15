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
 *         LED driver for ESP32-S3 platform
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#ifndef LEDS_ARCH_H_
#define LEDS_ARCH_H_

#include "contiki.h"

/* Platform-specific LED definitions */
#ifdef PLATFORM_CONF_LEDS_NUM
#define LEDS_NUM  PLATFORM_CONF_LEDS_NUM
#else
#define LEDS_NUM  1
#endif

/* LED bit masks - standard Contiki API */
#define LEDS_GREEN    1
#define LEDS_YELLOW   2
#define LEDS_RED      4
#define LEDS_BLUE     8

/* Alias for single LED boards */
#define LEDS_LED1     LEDS_GREEN

#define LEDS_ALL      (LEDS_GREEN | LEDS_YELLOW | LEDS_RED | LEDS_BLUE)

/**
 * \brief Initialize the LED driver
 */
void leds_arch_init(void);

/**
 * \brief Get current LED state
 * \return Bitmask of currently lit LEDs
 */
unsigned char leds_arch_get(void);

/**
 * \brief Set LED state
 * \param leds Bitmask of LEDs to turn on
 */
void leds_arch_set(unsigned char leds);

#endif /* LEDS_ARCH_H_ */
