/**
 * WiFi UDP Sender - IPv4 Version (Contiki-based)
 * Uses Contiki processes with wifi-manager and udp-manager HAL
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "contiki.h"
#include "sys/log.h"
#include "sys/etimer.h"
#include "sys/process.h"

#include "wifi-manager.h"
#include "udp-manager.h"

#define LOG_MODULE "Sender"
#define LOG_LEVEL LOG_LEVEL_INFO

/* WiFi Configuration */
#define WIFI_SSID "Victor"
#define WIFI_PASSWORD "victor1234"

/* Network Configuration */
#define RECEIVER_IP "192.168.43.93"
#define UDP_PORT 5678
#define SEND_INTERVAL (5 * CLOCK_SECOND)

/* Message structure - must match receiver */
typedef struct {
  uint32_t seq_num;
  uint32_t timestamp;
  float temperature;
  float humidity;
  char device_id[16];
  char message[64];
} sensor_data_t;

/* Global state */
static int wifi_connected = 0;
static udp_connection_t udp_conn;
static uint32_t receiver_addr;
static uint32_t seq_num = 0;

/*---------------------------------------------------------------------------*/
/* Sender Process */
/*---------------------------------------------------------------------------*/
PROCESS(udp_sender_process, "UDP Sender Process");
AUTOSTART_PROCESSES(&udp_sender_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_sender_process, ev, data)
{
  static struct etimer send_timer;
  static struct etimer wifi_timer;
  sensor_data_t sensor_msg;
  
  PROCESS_BEGIN();

  LOG_INFO("UDP Sender starting\n");
  
  /* Connect to WiFi */
  if(wifi_manager_connect(WIFI_SSID, WIFI_PASSWORD) < 0) {
    LOG_ERR("Failed to connect to WiFi\n");
  }
  
  /* Parse receiver IP address */
  receiver_addr = udp_ip4addr_from_string(RECEIVER_IP);
  if(receiver_addr == 0) {
    LOG_ERR("Invalid receiver IP: %s\n", RECEIVER_IP);
    PROCESS_EXIT();
  }
  
  /* Wait for WiFi to be ready */
  LOG_INFO("Waiting for WiFi connection...\n");
  etimer_set(&wifi_timer, 2 * CLOCK_SECOND);
  
  while(!wifi_connected) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&wifi_timer));
    etimer_reset(&wifi_timer);
    
    wifi_status_t status = wifi_manager_get_status();
    if(status == WIFI_STATUS_CONNECTED) {
      wifi_connected = 1;
      
      wifi_info_t info;
      if(wifi_manager_get_info(&info) == 0) {
        char ip_str[16];
        udp_ip4addr_to_string(info.ip_addr, ip_str);
        LOG_INFO("WiFi connected, IP: %s\n", ip_str);
      }
    }
  }
  
  /* Register UDP socket for sending */
  if(udp_register(&udp_conn, 0, NULL, 0, NULL) < 0) {
    LOG_ERR("Failed to register UDP connection\n");
    PROCESS_EXIT();
  }
  
  char receiver_str[16];
  udp_ip4addr_to_string(receiver_addr, receiver_str);
  LOG_INFO("UDP sender registered, target: %s:%u\n", receiver_str, UDP_PORT);
  
  /* Start transmission timer */
  etimer_set(&send_timer, SEND_INTERVAL);
  
  /* Main event loop */
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
    etimer_reset(&send_timer);
    
    /* Prepare sensor data */
    seq_num++;
    sensor_msg.seq_num = seq_num;
    sensor_msg.timestamp = clock_seconds();
    
    /* Simulate sensor readings */
    sensor_msg.temperature = 20.0f + (seq_num % 10);
    sensor_msg.humidity = 50.0f + (seq_num % 20);
    
    strncpy(sensor_msg.device_id, "ESP32-S3-01", sizeof(sensor_msg.device_id) - 1);
    sensor_msg.device_id[sizeof(sensor_msg.device_id) - 1] = '\0';
    
    snprintf(sensor_msg.message, sizeof(sensor_msg.message), "Packet #%lu", seq_num);
    
    /* Send data */
    int bytes_sent = udp_send_data(&udp_conn, &sensor_msg, sizeof(sensor_msg),
                                   &receiver_addr, UDP_PORT);
    
    if(bytes_sent < 0) {
      LOG_WARN("Send failed\n");
    } else {
      LOG_INFO("TX [%lu]: T=%.1f H=%.1f (%d bytes)\n",
               seq_num, sensor_msg.temperature, sensor_msg.humidity, bytes_sent);
    }
  }

  PROCESS_END();
}


