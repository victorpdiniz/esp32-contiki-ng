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
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *         Logging wrapper for ESP32-S3 - Maps Contiki LOG macros to ESP-IDF
 * \author
 *         Contiki-NG ESP32-S3 Port
 */

#ifndef SYS_LOG_H_
#define SYS_LOG_H_

#include "esp_log.h"

/* Contiki log level definitions */
#define LOG_LEVEL_NONE  0
#define LOG_LEVEL_ERR   1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_DBG   4

/* Map Contiki LOG macros to ESP_LOG */
#define LOG_INFO(...)  ESP_LOGI(LOG_MODULE, __VA_ARGS__)
#define LOG_WARN(...)  ESP_LOGW(LOG_MODULE, __VA_ARGS__)
#define LOG_ERR(...)   ESP_LOGE(LOG_MODULE, __VA_ARGS__)
#define LOG_DBG(...)   ESP_LOGD(LOG_MODULE, __VA_ARGS__)

/* Default LOG_MODULE if not defined */
#ifndef LOG_MODULE
#define LOG_MODULE "Contiki"
#endif

/* Note: LOG_LEVEL should be defined per-module, not globally */

#endif /* SYS_LOG_H_ */
