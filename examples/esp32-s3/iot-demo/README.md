# iot-demo

Contiki-NG IoT demo with WiFi, sensors, and LED. Shows three concurrent processes.

## Setup

Edit WiFi credentials in `main/iot-app.c`:
```c
#define WIFI_SSID     "YourSSID"
#define WIFI_PASSWORD "YourPassword"
```

## Build and Run

```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Output

```
Connecting to WiFi: YourSSID
WiFi Connected! IP Address: 192.168.1.100

=== Reading cycle #1 ===
Temperature: 23.50 °C
Humidity: 55.20 %
```
