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
 *         UDP Manager for ESP32-S3 - Contiki integration layer
 * \author
 *         Contiki-NG ESP32-S3 Port
 * 
 * \brief UDP abstraction layer wrapping lwIP sockets with Contiki interfaces
 *        Provides simple-udp-like callback registration for IPv4 UDP
 */

#ifndef UDP_MANAGER_H_
#define UDP_MANAGER_H_

#include "contiki.h"
#include <stdint.h>

/*---------------------------------------------------------------------------*/
/* UDP connection structure - similar to simple_udp_connection */
typedef struct udp_connection {
  uint16_t local_port;
  uint16_t remote_port;
  
  /* Callback function for received data */
  void (*callback)(struct udp_connection *c,
                   const uint32_t *remote_addr,
                   uint16_t remote_port,
                   const uint8_t *data,
                   uint16_t datalen);
  
  /* Internal socket file descriptor */
  int sock;
  
  /* Process to wake up on data reception */
  struct process *callback_process;
} udp_connection_t;

/*---------------------------------------------------------------------------*/
/**
 * \brief Initialize UDP manager
 * 
 * Must be called once before using any UDP functions
 */
void udp_manager_init(void);

/**
 * \brief Register UDP connection with callback
 * 
 * \param c Pointer to udp_connection structure to initialize
 * \param local_port Local port to bind to
 * \param remote_addr Remote address to send to (NULL for any), stored as IPv4 address
 * \param remote_port Remote port for sending (0 if not used)
 * \param callback Function called when data is received
 * 
 * \return 0 on success, -1 on failure
 */
int udp_register(udp_connection_t *c,
                 uint16_t local_port,
                 const uint32_t *remote_addr,
                 uint16_t remote_port,
                 void (*callback)(struct udp_connection *c,
                                  const uint32_t *remote_addr,
                                  uint16_t remote_port,
                                  const uint8_t *data,
                                  uint16_t datalen));

/**
 * \brief Unregister UDP connection
 * 
 * \param c Pointer to udp_connection to close
 */
void udp_unregister(udp_connection_t *c);

/**
 * \brief Send data to remote host
 * 
 * \param c UDP connection to send from
 * \param data Pointer to data buffer
 * \param datalen Length of data to send
 * \param remote_addr Remote IPv4 address to send to
 * \param remote_port Remote port to send to
 * 
 * \return Number of bytes sent, or -1 on error
 */
int udp_send_data(udp_connection_t *c,
                  const void *data,
                  uint16_t datalen,
                  const uint32_t *remote_addr,
                  uint16_t remote_port);

/**
 * \brief Convert IPv4 address string to uint32_t
 * 
 * \param addr String like "192.168.1.1"
 * \return IPv4 address as uint32_t in network byte order, or 0 on error
 */
uint32_t udp_ip4addr_from_string(const char *addr);

/**
 * \brief Convert uint32_t IPv4 address to string
 * 
 * \param addr IPv4 address as uint32_t in network byte order
 * \param buf Buffer to store string (at least 16 bytes)
 * 
 * \return Pointer to buf
 */
char *udp_ip4addr_to_string(uint32_t addr, char *buf);

/*---------------------------------------------------------------------------*/
#endif /* UDP_MANAGER_H_ */
