/**
 * WiFi UDP Receiver - Contiki-NG Process
 */

#include "contiki.h"
#include "sys/log.h"
#include "sys/etimer.h"

#include "wifi-manager.h"
#include "udp-manager.h"

#define LOG_MODULE "Receiver"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WIFI_SSID "DiscoveryNet_Andreia"
#define WIFI_PASSWORD "cipc@es01"
#define UDP_PORT 5678
#define STATS_INTERVAL (30 * CLOCK_SECOND)
#define MAX_CLIENTS 10

typedef struct {
  uint32_t seq_num;
  uint32_t timestamp;
  float temperature;
  float humidity;
  char device_id[16];
  char message[64];
} sensor_data_t;

typedef struct {
  uint32_t addr;
  uint32_t packets;
  uint32_t last_seq;
} client_info_t;

/*---------------------------------------------------------------------------*/
PROCESS(udp_receiver_process, "UDP Receiver");
PROCESS(stats_process, "Stats");
AUTOSTART_PROCESSES(&udp_receiver_process, &stats_process);

static client_info_t clients[MAX_CLIENTS];
static uint8_t num_clients = 0;
static udp_connection_t udp_conn;

/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(udp_connection_t *c, const uint32_t *remote_addr,
                uint16_t remote_port, const uint8_t *data, uint16_t datalen)
{
  sensor_data_t *sensor_data = (sensor_data_t *)data;
  uint8_t i;
  
  if(datalen != sizeof(sensor_data_t)) {
    LOG_WARN("Malformed packet (%u bytes)\n", datalen);
    return;
  }
  
  /* Find or add client */
  for(i = 0; i < num_clients; i++) {
    if(clients[i].addr == *remote_addr) {
      clients[i].packets++;
      clients[i].last_seq = sensor_data->seq_num;
      
      char ip_str[16];
      udp_ip4addr_to_string(*remote_addr, ip_str);
      LOG_INFO("RX from %s [%lu]: T=%.1f H=%.1f\n",
               ip_str, sensor_data->seq_num, 
               sensor_data->temperature, sensor_data->humidity);
      return;
    }
  }
  
  if(num_clients < MAX_CLIENTS) {
    clients[num_clients].addr = *remote_addr;
    clients[num_clients].packets = 1;
    clients[num_clients].last_seq = sensor_data->seq_num;
    num_clients++;
    
    char ip_str[16];
    udp_ip4addr_to_string(*remote_addr, ip_str);
    LOG_INFO("New client: %s [%lu]: T=%.1f H=%.1f\n",
             ip_str, sensor_data->seq_num, 
             sensor_data->temperature, sensor_data->humidity);
  }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_receiver_process, ev, data)
{
  static struct etimer wifi_timer;
  static int wifi_connected = 0;

  PROCESS_BEGIN();

  LOG_INFO("UDP Receiver starting\n");
  
  wifi_manager_connect(WIFI_SSID, WIFI_PASSWORD);
  etimer_set(&wifi_timer, 2 * CLOCK_SECOND);
  
  while(!wifi_connected) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&wifi_timer));
    etimer_reset(&wifi_timer);
    
    if(wifi_manager_get_status() == WIFI_STATUS_CONNECTED) {
      wifi_connected = 1;
      wifi_info_t info;
      if(wifi_manager_get_info(&info) == 0) {
        char ip_str[16];
        udp_ip4addr_to_string(info.ip_addr, ip_str);
        LOG_INFO("WiFi connected, IP: %s\n", ip_str);
      }
    }
  }
  
  if(udp_register(&udp_conn, UDP_PORT, NULL, 0, udp_rx_callback) < 0) {
    LOG_ERR("UDP register failed\n");
    PROCESS_EXIT();
  }
  
  LOG_INFO("UDP listening on port %u\n", UDP_PORT);
  
  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(stats_process, ev, data)
{
  static struct etimer stats_timer;
  uint8_t i;

  PROCESS_BEGIN();

  LOG_INFO("Stats process starting\n");
  etimer_set(&stats_timer, STATS_INTERVAL);
  
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&stats_timer));
    etimer_reset(&stats_timer);
    
    LOG_INFO("=== Clients: %d ===\n", num_clients);
    
    for(i = 0; i < num_clients; i++) {
      char ip_str[16];
      udp_ip4addr_to_string(clients[i].addr, ip_str);
      LOG_INFO("  %s - %lu packets (seq: %lu)\n",
               ip_str, clients[i].packets, clients[i].last_seq);
    }
  }

  PROCESS_END();
}