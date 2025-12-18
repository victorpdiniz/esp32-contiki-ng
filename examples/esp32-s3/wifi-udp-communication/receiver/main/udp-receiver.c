/**
 * WiFi UDP Receiver - IPv4 Version (Contiki-based)
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

#define LOG_MODULE "Receiver"
#define LOG_LEVEL LOG_LEVEL_INFO

/* WiFi Configuration */
#define WIFI_SSID "Victor"
#define WIFI_PASSWORD "victor1234"

/* Network Configuration */
#define UDP_PORT 5678
#define STATS_INTERVAL (30 * CLOCK_SECOND)
#define MAX_CLIENTS 10

/* Message structure - must match sender */
typedef struct {
  uint32_t seq_num;
  uint32_t timestamp;
  float temperature;
  float humidity;
  char device_id[16];
  char message[64];
} sensor_data_t;

/* Client tracking */
typedef struct {
  uint32_t addr;  /* IPv4 address as uint32_t */
  uint32_t packets_received;
  uint32_t last_seq;
  uint32_t last_seen;
} client_info_t;

/* Global state */
static client_info_t clients[MAX_CLIENTS];
static uint8_t num_clients = 0;
static udp_connection_t udp_conn;
static int wifi_connected = 0;

/*---------------------------------------------------------------------------*/
/* Client management functions */
/*---------------------------------------------------------------------------*/
static client_info_t* find_or_add_client(uint32_t addr)
{
  uint8_t i;
  uint32_t current_time = clock_seconds();
  
  /* Search for existing client */
  for(i = 0; i < num_clients; i++) {
    if(clients[i].addr == addr) {
      return &clients[i];
    }
  }
  
  /* Add new client if space available */
  if(num_clients < MAX_CLIENTS) {
    clients[num_clients].addr = addr;
    clients[num_clients].packets_received = 0;
    clients[num_clients].last_seq = 0;
    clients[num_clients].last_seen = current_time;
    return &clients[num_clients++];
  }
  
  return NULL;
}

/*---------------------------------------------------------------------------*/
/* UDP Callback - Called when data is received */
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(udp_connection_t *c,
                const uint32_t *remote_addr,
                uint16_t remote_port,
                const uint8_t *data,
                uint16_t datalen)
{
  sensor_data_t *sensor_data = (sensor_data_t *)data;
  
  if(datalen == sizeof(sensor_data_t)) {
    /* Find or add client */
    client_info_t *client = find_or_add_client(*remote_addr);
    
    if(client) {
      client->packets_received++;
      client->last_seq = sensor_data->seq_num;
      client->last_seen = clock_seconds();
      
      char ip_str[16];
      udp_ip4addr_to_string(*remote_addr, ip_str);
      
      LOG_INFO("RX from %s [%lu]: T=%.1f H=%.1f (%u bytes)\n",
               ip_str, sensor_data->seq_num, 
               sensor_data->temperature, sensor_data->humidity, datalen);
    }
  } else {
    LOG_WARN("Received malformed packet (%u bytes)\n", datalen);
  }
}

/*---------------------------------------------------------------------------*/
/* Main Receiver Process */
/*---------------------------------------------------------------------------*/
PROCESS(udp_receiver_process, "UDP Receiver Process");
PROCESS(stats_process, "Statistics Process");
AUTOSTART_PROCESSES(&udp_receiver_process, &stats_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_receiver_process, ev, data)
{
  static struct etimer wifi_timer;
  static struct etimer rx_timer;
  
  PROCESS_BEGIN();

  LOG_INFO("UDP Receiver starting\n");
  
  /* Connect to WiFi */
  if(wifi_manager_connect(WIFI_SSID, WIFI_PASSWORD) < 0) {
    LOG_ERR("Failed to connect to WiFi\n");
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
  
  /* Register UDP connection */
  if(udp_register(&udp_conn, UDP_PORT, NULL, 0, udp_rx_callback) < 0) {
    LOG_ERR("Failed to register UDP connection\n");
    PROCESS_EXIT();
  }
  
  LOG_INFO("UDP receiver listening on port %u\n", UDP_PORT);
  
  /* Main event loop - process just waits for UDP reception */
  while(1) {
    PROCESS_WAIT_EVENT();
    /* UDP reception happens in callback, triggered by FreeRTOS task */
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(stats_process, ev, data)
{
  static struct etimer stats_timer;
  uint8_t i;
  
  PROCESS_BEGIN();

  LOG_INFO("Statistics process starting\n");
  
  etimer_set(&stats_timer, STATS_INTERVAL);
  
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&stats_timer));
    etimer_reset(&stats_timer);
    
    LOG_INFO("\n=== Statistics ===\n");
    LOG_INFO("Active clients: %d\n", num_clients);
    
    for(i = 0; i < num_clients; i++) {
      char ip_str[16];
      udp_ip4addr_to_string(clients[i].addr, ip_str);
      
      LOG_INFO("  [%d] %s - %lu packets (last seq: %lu)\n",
               i, ip_str, clients[i].packets_received, clients[i].last_seq);
    }
  }
  
  PROCESS_END();
}


