# ESP32-S3 Contiki-NG Multi-Task Architecture

## 🏗️ Architecture Overview

This project implements a sophisticated multi-layered architecture combining **FreeRTOS** for hardware management and **Contiki-NG** for IoT application logic.

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
│              (Contiki-NG Processes)                         │
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │ LED Blink    │  │ Data         │  │ Network      │       │
│  │ Process      │  │ Collector    │  │ Process      │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
│                          ↕                                  │
│               FreeRTOS-Contiki Bridge                       │
│                          ↕                                  │
├─────────────────────────────────────────────────────────────┤
│                   Hardware Layer                            │
│                (FreeRTOS Tasks)                             │
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │ WiFi Task    │  │ Sensor Task  │  │ Contiki Task │       │
│  │ (ESP-IDF)    │  │ (Periodic)   │  │ (Main Loop)  │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
└─────────────────────────────────────────────────────────────┘
```

## 📋 Components

### 1. **FreeRTOS-Contiki Bridge** (`freertos-bridge.c/h`)

**Purpose**: Enable communication between FreeRTOS tasks and Contiki processes

**Features**:
- Thread-safe event posting from FreeRTOS to Contiki
- Mutex/semaphore wrappers for shared resource access
- Queue-based message passing
- Automatic memory management for event data

**Key Functions**:
```c
// Post event from FreeRTOS task to Contiki process
int freertos_post_to_contiki(struct process *process, 
                              process_event_t event,
                              const void *data,
                              size_t data_len);

// Create mutex for shared resource protection
SemaphoreHandle_t freertos_create_mutex(void);

// Take/Give mutex with timeout
BaseType_t freertos_take_mutex(SemaphoreHandle_t mutex, uint32_t timeout_ms);
void freertos_give_mutex(SemaphoreHandle_t mutex);
```

**Usage Example**:
```c
// From FreeRTOS task
sensor_data_t data = {.type = SENSOR_TYPE_TEMP, .value = 25.5};
freertos_post_to_contiki(&my_process, sensor_event, &data, sizeof(data));

// In Contiki process
PROCESS_THREAD(my_process, ev, data) {
  PROCESS_BEGIN();
  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == sensor_event) {
      sensor_data_t *sensor = (sensor_data_t *)data;
      // Process sensor data
    }
  }
  PROCESS_END();
}
```

---

### 2. **WiFi Manager** (`wifi-manager.c/h`)

**Purpose**: Manage WiFi connectivity using ESP-IDF APIs with Contiki integration

**Features**:
- Station mode WiFi connection
- Automatic event translation (ESP-IDF → Contiki)
- Connection monitoring and auto-retry
- Signal strength (RSSI) tracking

**Architecture**:
```
ESP-IDF WiFi Stack → Event Handler → Bridge → Contiki Process
```

**Key Functions**:
```c
void wifi_manager_init(void);
int wifi_manager_connect(const char *ssid, const char *password);
wifi_status_t wifi_manager_get_status(void);
int wifi_manager_get_info(wifi_info_t *info);
```

**Events Posted**:
- `wifi_event_connected` - WiFi associated with AP
- `wifi_event_disconnected` - WiFi connection lost
- `wifi_event_got_ip` - IP address obtained via DHCP

**Usage Example**:
```c
PROCESS_THREAD(network_process, ev, data) {
  PROCESS_BEGIN();
  
  wifi_manager_init();
  wifi_manager_connect("MySSID", "MyPassword");
  
  while(1) {
    PROCESS_WAIT_EVENT();
    
    if(ev == wifi_event_connected) {
      printf("WiFi connected!\n");
    } else if(ev == wifi_event_got_ip) {
      wifi_info_t *info = (wifi_info_t *)data;
      printf("Got IP: %d.%d.%d.%d\n", IP_BYTES(info->ip_addr));
    }
  }
  
  PROCESS_END();
}
```

---

### 3. **Sensor Manager** (`sensor-manager.c/h`)

**Purpose**: Periodic sensor reading in FreeRTOS task with Contiki notification

**Features**:
- Dedicated FreeRTOS task for sensor polling
- Mutex-protected sensor data cache
- Automatic event posting to Contiki
- Supports multiple sensor types

**Architecture**:
```
FreeRTOS Task (periodic) → Read Sensors → Update Cache → Post Event → Contiki Process
```

**Sensor Types**:
- Temperature
- Humidity
- Pressure
- Light
- Motion

**Key Functions**:
```c
void sensor_manager_init(void);
int sensor_manager_read(sensor_type_t type, float *value);
int sensor_manager_get_last_reading(sensor_type_t type, sensor_data_t *data);
```

**Usage Example**:
```c
// Initialize (called once at startup)
sensor_manager_init();

// In Contiki process - receive events
PROCESS_THREAD(data_process, ev, data) {
  PROCESS_BEGIN();
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensor_event_data_ready);
    
    sensor_data_t *sensor = (sensor_data_t *)data;
    printf("Sensor %d: %.2f\n", sensor->type, sensor->value);
  }
  PROCESS_END();
}

// Or poll current values directly
float temperature;
if(sensor_manager_read(SENSOR_TYPE_TEMPERATURE, &temperature) == 0) {
  printf("Current temp: %.2f °C\n", temperature);
}
```

---

## 🚀 Application Example: `iot-app.c`

The example application demonstrates all architectural features:

### **Process 1: LED Blink Process**
- Basic protothread operation
- Periodic timer usage
- GPIO control from Contiki

### **Process 2: Data Collector Process**
- Event-driven sensor data reception
- Periodic data aggregation
- Demonstrates both push (events) and pull (polling) models

### **Process 3: Network Process**
- WiFi connection management
- Event handling for network state changes
- Foundation for network services (HTTP, MQTT, CoAP)

---

## 🔧 How to Build and Flash

### Prerequisites
```bash
# Install ESP-IDF
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32s3

# Activate ESP-IDF environment
. ~/esp/esp-idf/export.sh
```

### Build Steps
```bash
cd /home/victorpdiniz/contiki-ng/examples/esp32-s3/hello-world

# Configure WiFi credentials (edit main/iot-app.c)
# Update WIFI_SSID and WIFI_PASSWORD

# Build
idf.py build

# Flash to ESP32-S3
idf.py -p /dev/ttyUSB0 flash

# Monitor output
idf.py -p /dev/ttyUSB0 monitor
```

### Expected Output
```
I (123) Contiki-NG: Platform Init Stage One
I (234) FreeRTOS-Bridge: FreeRTOS-Contiki bridge initialized
I (345) WiFi-Manager: WiFi Manager initialized
I (456) Sensor-Manager: Sensor Manager initialized
I (567) IoT-App: LED Blink Process started
I (678) IoT-App: Data Collector Process started
I (789) IoT-App: Network Process started
I (890) IoT-App: Connecting to WiFi SSID: YourSSID
I (1234) WiFi-Manager: Connected to AP SSID:YourSSID channel:6
I (2345) WiFi-Manager: Got IP:192.168.1.100
I (2456) IoT-App: 🎉 Network Ready!
I (2567) IoT-App:   IP: 192.168.1.100
I (3456) Sensor-Manager: Temperature: 24.32 °C
I (3567) IoT-App: 📊 Sensor Data Received: Type: 0, Value: 24.32
```

---

## 🎯 Key Concepts Explained

### 1. **Why Contiki on FreeRTOS?**

| Layer | Responsibility | Why |
|-------|---------------|-----|
| **FreeRTOS** | Hardware, WiFi, Bluetooth, Real-time | ESP-IDF provides optimized drivers |
| **Contiki-NG** | IoT protocols, Application logic | Event-driven, lightweight, IoT-focused |

### 2. **Protothreads vs Traditional Threads**

Contiki processes are **protothreads** - stackless cooperative threads:

```c
PROCESS_THREAD(example, ev, data) {
  static int i;  // Must be static!
  PROCESS_BEGIN();
  
  for(i = 0; i < 10; i++) {
    PROCESS_WAIT_EVENT();  // Yields here
    printf("%d\n", i);
  }
  
  PROCESS_END();
}
```

**Benefits**:
- Very low memory (no stack per process)
- Deterministic behavior
- No race conditions (cooperative)

**Limitations**:
- Cannot use blocking calls
- Local variables must be static
- Cannot use switch/case in user code

### 3. **Event Flow**

```
FreeRTOS Task                    Bridge                  Contiki Process
─────────────                    ──────                  ───────────────
    │                               │                           │
    │ Read sensor                   │                           │
    ├──────────────────────────────>│                           │
    │ freertos_post_to_contiki()    │                           │
    │                               ├──────────────────────────>│
    │                               │ process_post()            │
    │                               │                           │ PROCESS_WAIT_EVENT()
    │                               │                           ├─> Process sensor data
```

### 4. **Using FreeRTOS Resources from Contiki**

```c
// Create mutex (once at initialization)
static SemaphoreHandle_t my_mutex;
my_mutex = freertos_create_mutex();

// In Contiki process
PROCESS_THREAD(my_process, ev, data) {
  PROCESS_BEGIN();
  
  // Take mutex before accessing shared resource
  if(freertos_take_mutex(my_mutex, 100) == pdTRUE) {
    // Access shared resource
    shared_data++;
    
    // Release mutex
    freertos_give_mutex(my_mutex);
  }
  
  PROCESS_END();
}
```

---

## 📦 File Structure

```
arch/platform/esp32-s3/
├── platform.c              # Main platform initialization
├── freertos-bridge.c/h     # FreeRTOS ↔ Contiki bridge
├── wifi-manager.c/h        # WiFi management
├── sensor-manager.c/h      # Sensor reading
└── CMakeLists.txt          # ESP32-S3 component build

examples/esp32-s3/hello-world/
├── main/
│   ├── iot-app.c           # Main application
│   └── CMakeLists.txt      # Application build
├── CMakeLists.txt          # Project config
└── sdkconfig.defaults      # ESP-IDF defaults
```

---

## 🔨 Customization Guide

### Adding New Sensors

1. **Add sensor type** in `sensor-manager.h`:
```c
typedef enum {
  SENSOR_TYPE_TEMPERATURE,
  SENSOR_TYPE_CUSTOM,  // Your new sensor
} sensor_type_t;
```

2. **Implement reading** in `sensor-manager.c`:
```c
static float read_sensor_hw(sensor_type_t type) {
  if(type == SENSOR_TYPE_CUSTOM) {
    // Your sensor reading code
    return read_custom_sensor();
  }
}
```

3. **Use in application**:
```c
float custom_value;
sensor_manager_read(SENSOR_TYPE_CUSTOM, &custom_value);
```

### Adding New Processes

```c
PROCESS(my_new_process, "My New Process");
AUTOSTART_PROCESSES(&led_blink_process, 
                    &data_collector_process,
                    &network_process,
                    &my_new_process);  // Add here

PROCESS_THREAD(my_new_process, ev, data) {
  PROCESS_BEGIN();
  // Your code
  PROCESS_END();
}
```

### Creating FreeRTOS Tasks

```c
void my_task(void *pvParameters) {
  while(1) {
    // Do work
    
    // Post to Contiki
    my_data_t data = {.value = 123};
    freertos_post_to_contiki(&target_process, 
                            my_event,
                            &data, sizeof(data));
    
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// In initialization
xTaskCreate(my_task, "my_task", 4096, NULL, 3, NULL);
```

---

## 🐛 Troubleshooting

### Issue: Events not received in Contiki process

**Check**:
1. Bridge initialized? (`freertos_bridge_init()` called)
2. Bridge events processed? (`freertos_bridge_process_events()` in main loop)
3. Event allocated? (`my_event = process_alloc_event()`)

### Issue: Mutex deadlock

**Solution**:
- Always use timeout with `freertos_take_mutex()`
- Never call blocking FreeRTOS functions from Contiki processes
- Use `PROCESS_WAIT_EVENT()` instead of blocking

### Issue: Stack overflow

**Solution**:
- Increase task stack size in `xTaskCreate()`
- For Contiki: Use `static` for large local variables
- Check `CONTIKI_TASK_STACK_SIZE` in `platform.c`

---

## 📚 Further Reading

- [Contiki-NG Processes](https://docs.contiki-ng.org/en/develop/doc/programming/Processes.html)
- [FreeRTOS Tasks](https://www.freertos.org/taskandcr.html)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)

---

## 🎉 What You've Built

You now have a production-ready IoT architecture with:

✅ **Multi-task system** with FreeRTOS and Contiki-NG  
✅ **WiFi connectivity** with automatic event handling  
✅ **Sensor reading** in dedicated FreeRTOS task  
✅ **Inter-task communication** via bridge layer  
✅ **Event-driven** Contiki processes  
✅ **Resource sharing** with mutex protection  
✅ **Extensible** architecture for adding features

**Next Steps**:
- Add MQTT client for cloud connectivity
- Implement CoAP server for IoT communication
- Add data persistence with NVS
- Create REST API with HTTP server
- Implement OTA (Over-The-Air) updates

Happy coding! 🚀
