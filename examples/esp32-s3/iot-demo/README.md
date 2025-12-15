# ESP32-S3 IoT Demo Application

Advanced IoT demonstration showing WiFi connectivity, sensor reading, and multi-task architecture.

## Features

- **WiFi Management**: Station mode with automatic reconnection
- **Sensor Reading**: Periodic sensor data collection (simulated)
- **Multi-task Architecture**: FreeRTOS tasks + Contiki-NG processes
- **Event-driven Communication**: Bridge between FreeRTOS and Contiki

## Architecture

This example demonstrates:
- 3 Contiki-NG processes (LED blink, data collector, network)
- 2 FreeRTOS tasks (sensor reading, WiFi management)
- Inter-task event queue via freertos-bridge

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed system design.

## Quick Start

### Configure WiFi
Edit `main/iot-app.c` lines 65-66:
```c
#define WIFI_SSID     "YourSSID"
#define WIFI_PASSWORD "YourPassword"
```

### Build and Flash
```bash
# Configure for ESP32-S3
idf.py set-target esp32s3

# Build
idf.py build

# Flash and monitor
idf.py flash monitor
```

Or use the helper script:
```bash
./build.sh build
./build.sh flash
./build.sh monitor
```

## What You'll See

1. **LED Blinks**: GPIO 35 toggles every 1 second
2. **Sensor Data**: Temperature/humidity readings every 5 seconds
3. **WiFi Events**: Connection status, IP address, RSSI
4. **Data Collection**: Periodic polling of cached sensor values

## Customization

### Add Real Sensors
Replace simulated sensors in `arch/platform/esp32-s3/components/sensor-manager.c`:
```c
static float read_sensor_hw(sensor_type_t type) {
  // Replace with actual I2C/SPI sensor driver
}
```

### Add IoT Protocol
Example: MQTT client
```c
#include "net/app-layer/mqtt/mqtt.h"

PROCESS(mqtt_process, "MQTT Process");
// ... implement MQTT connection and publishing
```

## Files

- `main/iot-app.c` - Application with 3 Contiki processes
- `ARCHITECTURE.md` - System architecture diagrams
- `IMPLEMENTATION_SUMMARY.md` - Complete API reference
- `DIAGRAMS.md` - Visual system diagrams

## See Also

- [hello-world](../hello-world/) - Minimal Contiki-NG example
- [ESP32-S3 Platform README](../../../arch/platform/esp32-s3/README.md)
