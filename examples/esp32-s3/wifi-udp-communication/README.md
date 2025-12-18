# WiFi UDP Communication Example

Demonstrates IPv4 UDP communication over WiFi between ESP32-S3 devices using FreeRTOS and lwIP.

## Project Structure

```
wifi-udp-communication/
├── sender/              # UDP sender application
│   ├── main/udp-sender.c
│   ├── CMakeLists.txt
│   └── sdkconfig.defaults
└── receiver/            # UDP receiver application
    ├── main/udp-receiver.c
    ├── CMakeLists.txt
    └── sdkconfig.defaults
```

## Quick Start

### Setup

1. **Configure WiFi** - Edit both applications:
   ```c
   // In sender/main/udp-sender.c and receiver/main/udp-receiver.c
   #define WIFI_SSID "YourSSID"
   #define WIFI_PASSWORD "YourPassword"
   ```

2. **Get Receiver IP** - After first receiver boot, note its IP from serial output

3. **Update Sender IP**:
   ```c
   // In sender/main/udp-sender.c
   #define RECEIVER_IP "192.168.x.x"  // From receiver serial output
   ```

### Build & Flash

**Receiver** (USB1):
```bash
cd receiver
source ~/esp/v5.5.1/esp-idf/export.sh
idf.py build
idf.py -p /dev/ttyUSB1 flash monitor
```

**Sender** (USB0):
```bash
cd sender
source ~/esp/v5.5.1/esp-idf/export.sh
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Features

### Sender
- Sends simulated sensor data (temperature, humidity) every 5 seconds
- Auto-connects to WiFi
- Structured UDP packets
- Configurable send interval

### Receiver
- Listens on UDP port 5678
- Tracks multiple clients by IP
- Displays received data with sender info
- Periodic statistics (every 30 seconds)
- Multi-client support (up to 10 concurrent)

## Configuration

### Data Structure
Both apps use identical packet format:
```c
typedef struct {
  uint32_t seq_num;        // Sequence number
  uint32_t timestamp;      // Unix timestamp
  float temperature;       // °C
  float humidity;          // %
  char device_id[16];      // Device name
  char message[64];        // Custom message
} sensor_data_t;
```

### Network Settings
- **Protocol**: UDP over IPv4
- **Port**: 5678
- **WiFi**: 2.4GHz only
- **Stack**: lwIP + FreeRTOS

### Customization

**Send Interval** (sender):
```c
#define SEND_INTERVAL_MS 5000  // 5 seconds
```

**Max Clients** (receiver):
```c
#define MAX_CLIENTS 10  // Track up to 10 senders
```

## Serial Output Examples

### Receiver Boot
```
I (396) Receiver: WiFi UDP Receiver Starting
I (1686) Receiver: Got IP:192.168.43.93
I (1686) Receiver: Listening on port 5678
```

### Sender Transmitting
```
I (396) Sender: WiFi UDP Sender Starting
I (5686) Sender: TX [1]: T=21.0 H=51.0 (144 bytes)
I (10686) Sender: TX [2]: T=22.0 H=52.0 (144 bytes)
```

### Receiver Getting Data
```
I (5000) Receiver: RX from 192.168.43.100 [1]: T=21.0 H=51.0 (144 bytes)

=== Statistics ===
Active clients: 1
  [0] 192.168.43.100 - 5 packets (last seq: 5)
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| **PSRAM error on boot** | Disabled in sdkconfig.defaults (board has no PSRAM) |
| **WiFi won't connect** | Check SSID/password, ensure 2.4GHz network |
| **Sender can't reach receiver** | Verify receiver IP in sender code, check firewall |
| **Build fails** | Run `rm -rf build sdkconfig` then rebuild |
| **No serial output** | Check USB connection, try different USB port |

## Development Tips

- Edit source files in `sender/main/udp-sender.c` or `receiver/main/udp-receiver.c`
- Config files: `sdkconfig.defaults` (persistent settings)
- Each project builds independently - no rebuild when switching roles
- Use `idf.py monitor` to view serial output after flashing
- Press `Ctrl+]` in monitor to exit

## Architecture

**Sender Flow:**
1. Initialize WiFi via event handlers
2. Wait for IP assignment
3. Create UDP socket
4. Send sensor packets every 5 seconds
5. Simulate temperature/humidity readings

**Receiver Flow:**
1. Initialize WiFi via event handlers  
2. Wait for IP assignment
3. Create UDP socket and bind to port 5678
4. Non-blocking receive loop (50ms poll rate)
5. Track clients and display statistics every 30s

Both use FreeRTOS tasks and event handlers for WiFi management, lwIP sockets for communication.

## License

Part of Contiki-NG project
