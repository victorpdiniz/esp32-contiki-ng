# WiFi UDP Communication Example

Demonstrates UDP communication over WiFi between ESP32-S3 devices using Contiki-NG.

## Quick Start

### 1. Configure WiFi

Edit SSID/password in both sender and receiver:
```c
// sender/main/udp-sender.c and receiver/main/udp-receiver.c
#define WIFI_SSID "YourSSID"
#define WIFI_PASSWORD "YourPassword"
```

### 2. Build & Run

**Receiver:**
```bash
cd receiver
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```
Note the IP address shown (e.g., `192.168.1.100`)

**Sender:**
```bash
# Update receiver IP in sender/main/udp-sender.c:
#define RECEIVER_IP "192.168.1.100"  # Use receiver's IP

cd sender
idf.py build
idf.py -p /dev/ttyUSB1 flash monitor
```

## What It Does

**Sender:** Sends sensor data (temperature/humidity) every 5 seconds  
**Receiver:** Listens on port 5678, displays data from all senders

## Output Example

**Receiver:**
```
Got IP: 192.168.1.100
Listening on port 5678
RX from 192.168.1.101 [seq:5] T=22.5°C H=55.0%
```

**Sender:**
```
Got IP: 192.168.1.101
TX [seq:5] T=22.5°C H=55.0% to 192.168.1.100:5678
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| WiFi won't connect | Check SSID/password, use 2.4GHz network |
| No data received | Verify receiver IP in sender code |
| Build fails | `rm -rf build sdkconfig` then rebuild |

## Customization

**Change send interval** (sender/main/udp-sender.c):
```c
#define SEND_INTERVAL_MS 5000  // milliseconds
```

**Change port** (both files):
```c
#define UDP_PORT 5678
```
