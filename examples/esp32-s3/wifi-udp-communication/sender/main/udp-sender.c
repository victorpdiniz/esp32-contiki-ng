/**
 * WiFi UDP Sender - Contiki-NG Process
 */

#include "contiki.h"
#include "sys/log.h"
#include "sys/etimer.h"

#include "wifi-manager.h"
#include "udp-manager.h"

#define LOG_MODULE "Sender"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WIFI_SSID "DiscoveryNet_Andreia"
#define WIFI_PASSWORD "cipc@es01"
#define RECEIVER_IP "10.0.0.108"
#define UDP_PORT 5678
#define SEND_INTERVAL (5 * CLOCK_SECOND)

typedef struct {
  uint32_t seq_num;
  uint32_t timestamp;
  float temperature;
  float humidity;
  char device_id[16];
  char message[64];
} sensor_data_t;

/*---------------------------------------------------------------------------*/
PROCESS(udp_sender_process, "UDP Sender");
AUTOSTART_PROCESSES(&udp_sender_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_sender_process, ev, data)
{
  static struct etimer send_timer, wifi_timer;
  static udp_connection_t udp_conn;
  static uint32_t receiver_addr;
  static uint32_t seq_num = 0;
  static int wifi_connected = 0;
  sensor_data_t msg;
  int bytes_sent;

  PROCESS_BEGIN();

  LOG_INFO("UDP Sender starting\n");
  
  wifi_manager_connect(WIFI_SSID, WIFI_PASSWORD);
  receiver_addr = udp_ip4addr_from_string(RECEIVER_IP);
  
  if(receiver_addr == 0) {
    LOG_ERR("Invalid receiver IP: %s\n", RECEIVER_IP);
    PROCESS_EXIT();
  }
  
  /* Wait for WiFi connection */
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
  
  if(udp_register(&udp_conn, 0, NULL, 0, NULL) < 0) {
    LOG_ERR("UDP register failed\n");
    PROCESS_EXIT();
  }
  
  etimer_set(&send_timer, SEND_INTERVAL);
  
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
    etimer_reset(&send_timer);
    
    seq_num++;
    msg.seq_num = seq_num;
    msg.timestamp = clock_seconds();
    msg.temperature = 20.0f + (seq_num % 10);
    msg.humidity = 50.0f + (seq_num % 20);
    
    strncpy(msg.device_id, "ESP32-S3-01", sizeof(msg.device_id) - 1);
    snprintf(msg.message, sizeof(msg.message), "Packet #%lu", seq_num);
    
    bytes_sent = udp_send_data(&udp_conn, &msg, sizeof(msg), &receiver_addr, UDP_PORT);
    
    if(bytes_sent > 0) {
      LOG_INFO("TX [%lu]: T=%.1f H=%.1f (%d bytes)\n",
               seq_num, msg.temperature, msg.humidity, bytes_sent);
    }
  }

  PROCESS_END();
}



