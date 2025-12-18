# ESP32-S3 + Contiki-NG Architecture Diagrams

## System Architecture

```
╔═══════════════════════════════════════════════════════════════════╗
║                      APPLICATION LAYER                             ║
║                   (Contiki-NG Processes)                          ║
║                                                                   ║
║  ┌─────────────┐   ┌─────────────┐   ┌─────────────┐           ║
║  │   LED       │   │   Data      │   │  Network    │           ║
║  │   Blink     │   │  Collector  │   │  Process    │   ...     ║
║  │  (1s loop)  │   │ (Event+10s) │   │  (Events)   │           ║
║  └──────┬──────┘   └──────┬──────┘   └──────┬──────┘           ║
║         │                 │                  │                   ║
║         └─────────────────┼──────────────────┘                   ║
║                           │                                       ║
║                    Contiki Process                                ║
║                     Event System                                  ║
╚═══════════════════════════╬═══════════════════════════════════════╝
                            ║
                ┌───────────▼───────────┐
                │  FreeRTOS-Contiki     │
                │       Bridge          │
                │  ┌─────────────────┐  │
                │  │  Event Queue    │  │
                │  │  (16 messages)  │  │
                │  └─────────────────┘  │
                │  ┌─────────────────┐  │
                │  │  Mutex Manager  │  │
                │  └─────────────────┘  │
                └───────────┬───────────┘
                            ║
╔═══════════════════════════╩═══════════════════════════════════════╗
║                      HARDWARE LAYER                               ║
║                    (FreeRTOS Tasks)                               ║
║                                                                   ║
║  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐             ║
║  │   Contiki    │  │  WiFi Task   │  │ Sensor Task  │             ║
║  │     Loop     │  │  (ESP-IDF)   │  │  (Periodic)  │             ║
║  │  Priority 5  │  │  Priority 4  │  │  Priority 3  │             ║
║  │   8KB stack  │  │   4KB stack  │  │   4KB stack  │             ║
║  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘             ║
║         │                 │                 │                     ║
║         └─────────────────┼─────────────────┘                     ║
║                           │                                       ║
║                    FreeRTOS Kernel                                ║
║                  (Scheduler & Sync)                               ║
╚═══════════════════════════╬═══════════════════════════════════════╝
                            ║
╔═══════════════════════════╩═══════════════════════════════════════╗
║                    HARDWARE ABSTRACTION                           ║
║                         (ESP-IDF)                                 ║
║                                                                   ║
║  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐           ║
║  │   WiFi   │  │   GPIO   │  │   ADC    │  │  Timer   │           ║
║  │  Driver  │  │  Driver  │  │  Driver  │  │  Driver  │           ║
║  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘           ║
║       │             │             │             │                 ║
║       └─────────────┴─────────────┴─────────────┘                 ║
║                           │                                       ║
║                    Hardware Registers                             ║
╚═══════════════════════════════════════════════════════════════════╝
```

## Event Flow

```
┌─────────────────────────────────────────────────────────────┐
│  SENSOR READING FLOW                                        │
└─────────────────────────────────────────────────────────────┘

    FreeRTOS                Bridge              Contiki
    
    Sensor Task
        │
        │ 1. Read sensor
        ├────────────────────>
        │   read_sensor_hw()
        │
        │ 2. Store in cache
        │   (mutex protected)
        ├──────────┐
        │          │ mutex lock
        │          │ update cache
        │          │ mutex unlock
        │<─────────┘
        │
        │ 3. Post event          Bridge Queue
        ├───────────────────>┌──────────────┐
        │ freertos_post_to   │ Event Buffer │
        │ _contiki()         │ [sensor_data]│
        │                    └──────┬───────┘
        │                           │
        │                           │ 4. process_post()
        │                           ├────────────────>
        │                           │               Data Collector
        │                           │               Process
        │                           │                   │
        │                           │                   │ WAIT_EVENT
        │                           │                   ├────────────┐
        │                           │                   │ 5. Handle  │
        │                           │                   │    event   │
        │                           │                   │<───────────┘
        │                           │                   │
        │                           │                   │ 6. Process
        │                           │                   │    data
        │                           │                   │
        │                           │                   ▼
        
        
┌─────────────────────────────────────────────────────────────┐
│  WIFI CONNECTION FLOW                                       │
└─────────────────────────────────────────────────────────────┘

    ESP-IDF WiFi           WiFi Manager         Network Process
    
    WiFi Event
        │
        │ 1. WIFI_EVENT_STA_CONNECTED
        ├─────────────────>
        │                 wifi_event_handler()
        │                       │
        │                       │ 2. Update status
        │                       ├───────────┐
        │                       │           │
        │                       │<──────────┘
        │                       │
        │                       │ 3. Post to Contiki
        │                       ├──────────────────>
        │                       │  wifi_event_connected
        │                       │                     │
        │                       │                     │ WAIT_EVENT
        │                       │                     ├──────────┐
        │                       │                     │ 4. Log   │
        │                       │                     │ "WiFi    │
        │                       │                     │ Connect" │
        │                       │                     │<─────────┘
        │                       │                     │
    IP_EVENT_STA_GOT_IP       │                     │
        ├─────────────────>    │                     │
        │                       │                     │
        │                       │ 5. Post IP info     │
        │                       ├──────────────────>  │
        │                       │  wifi_event_got_ip  │
        │                       │  + ip_info_t        │
        │                       │                     │
        │                       │                     │ WAIT_EVENT
        │                       │                     ├──────────┐
        │                       │                     │ 6. Show  │
        │                       │                     │    IP    │
        │                       │                     │<─────────┘
        │                       │                     │
        ▼                       ▼                     ▼
```

## Process State Machine

```
LED Blink Process
─────────────────

    START
      │
      ├─> Configure GPIO
      │
      ├─> Set timer (1s)
      │
      ▼
   ┌──────────────────┐
   │  WAIT_EVENT      │◄─────────┐
   │  (etimer_expired)│          │
   └────────┬─────────┘          │
            │                     │
            │ Timer expired       │
            ▼                     │
      Toggle LED                  │
            │                     │
            ├─> GPIO = !GPIO      │
            │                     │
            ├─> Reset timer       │
            │                     │
            └─────────────────────┘


Data Collector Process
──────────────────────

    START
      │
      ├─> Wait 2s (init)
      │
      ├─> Set timer (10s)
      │
      ▼
   ┌─────────────────────┐
   │  WAIT_EVENT         │◄──────────┐
   │  (sensor or timer)  │           │
   └──────┬──────────────┘           │
          │                           │
          ├─> sensor_event?           │
          │   ├─> YES: Process data   │
          │   └─> Log sensor value    │
          │                           │
          ├─> timer_expired?          │
          │   ├─> YES: Poll sensors   │
          │   ├─> Read temperature    │
          │   ├─> Read humidity       │
          │   └─> Reset timer         │
          │                           │
          └───────────────────────────┘


Network Process
───────────────

    START
      │
      ├─> Init WiFi manager
      │
      ├─> Connect to AP
      │
      ▼
   ┌─────────────────────────┐
   │  WAIT_EVENT             │◄──────────────┐
   │  (wifi events)          │               │
   └──────┬──────────────────┘               │
          │                                   │
          ├─> wifi_connected?                │
          │   └─> Log "Connected"            │
          │                                   │
          ├─> wifi_disconnected?             │
          │   ├─> Log "Disconnected"         │
          │   ├─> Set retry timer (5s)       │
          │   └─> Retry connection           │
          │                                   │
          ├─> wifi_got_ip?                   │
          │   ├─> Extract IP info            │
          │   ├─> Log IP, RSSI, Channel      │
          │   └─> Ready for services         │
          │                                   │
          └───────────────────────────────────┘
```

## Memory Layout

```
┌─────────────────────────────────────────────────────────────┐
│                     ESP32-S3 Memory                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌───────────────────────────────────────────────────────┐ │
│  │  Flash Memory                                         │ │
│  │  ┌─────────────────────────────────────────────────┐ │ │
│  │  │  Bootloader                                     │ │ │
│  │  ├─────────────────────────────────────────────────┤ │ │
│  │  │  Partition Table                                │ │ │
│  │  ├─────────────────────────────────────────────────┤ │ │
│  │  │  NVS (Non-Volatile Storage)                     │ │ │
│  │  ├─────────────────────────────────────────────────┤ │ │
│  │  │  Application (Contiki-NG + ESP-IDF)            │ │ │
│  │  │  ├─ Platform code                              │ │ │
│  │  │  ├─ Bridge layer                               │ │ │
│  │  │  ├─ WiFi manager                               │ │ │
│  │  │  ├─ Sensor manager                             │ │ │
│  │  │  └─ Application processes                      │ │ │
│  │  └─────────────────────────────────────────────────┘ │ │
│  └───────────────────────────────────────────────────────┘ │
│                                                             │
│  ┌───────────────────────────────────────────────────────┐ │
│  │  SRAM (520 KB)                                        │ │
│  │  ┌─────────────────────────────────────────────────┐ │ │
│  │  │  FreeRTOS Kernel (~10 KB)                      │ │ │
│  │  ├─────────────────────────────────────────────────┤ │ │
│  │  │  Contiki Task Stack (8 KB)                     │ │ │
│  │  ├─────────────────────────────────────────────────┤ │ │
│  │  │  Sensor Task Stack (4 KB)                      │ │ │
│  │  ├─────────────────────────────────────────────────┤ │ │
│  │  │  WiFi Task Stack (4 KB)                        │ │ │
│  │  ├─────────────────────────────────────────────────┤ │ │
│  │  │  Bridge Queue (4 KB)                           │ │ │
│  │  ├─────────────────────────────────────────────────┤ │ │
│  │  │  WiFi Buffers (~20 KB)                         │ │ │
│  │  ├─────────────────────────────────────────────────┤ │ │
│  │  │  Heap (Dynamic allocation) (~100 KB)           │ │ │
│  │  ├─────────────────────────────────────────────────┤ │ │
│  │  │  BSS/Data (~30 KB)                             │ │ │
│  │  ├─────────────────────────────────────────────────┤ │ │
│  │  │  Available (~340 KB)                           │ │ │
│  │  └─────────────────────────────────────────────────┘ │ │
│  └───────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

## Timing Diagram

```
Time (seconds)
  0    1    2    3    4    5    6    7    8    9    10   11
  │────│────│────│────│────│────│────│────│────│────│────│
  │
LED Blink
  ├─ON─┤─OFF┤─ON─┤─OFF┤─ON─┤─OFF┤─ON─┤─OFF┤─ON─┤─OFF┤─ON─┤
  
Sensor Task
  ├────────────────────┼────────────────────┼────────────────
  │    Sleep 5s        │Read │  Sleep 5s    │Read │
  
Data Collector
  ├────────────────────────────────────────────┼────────────
  │         Wait 10s                           │Poll│
  
WiFi Events
  ├────┤
  │Conn│Got IP
  
Network Process
  │    └───┤──────────────────────────────────────────────
       Log    Idle (waiting for events)
```

## Component Dependencies

```
                    ┌─────────────────┐
                    │   iot-app.c     │
                    │  (Application)  │
                    └────────┬────────┘
                             │
                             │ uses
                ┌────────────┼────────────┐
                │            │            │
                ▼            ▼            ▼
        ┌──────────┐  ┌──────────┐  ┌──────────┐
        │  WiFi    │  │  Sensor  │  │ FreeRTOS │
        │ Manager  │  │ Manager  │  │  Bridge  │
        └────┬─────┘  └────┬─────┘  └────┬─────┘
             │             │             │
             │ depends on  │             │
             └─────────────┼─────────────┘
                           │
                           ▼
                    ┌─────────────┐
                    │  platform.c │
                    │  (Contiki   │
                    │   Runtime)  │
                    └──────┬──────┘
                           │
                           │ uses
                           ▼
                ┌──────────────────┐
                │    ESP-IDF       │
                │  (FreeRTOS +     │
                │   Drivers)       │
                └──────────────────┘
```

---

**Legend:**
- `┌─┐ └─┘` : Components/Modules
- `│ ─ ├ ┤ ┬ ┴ ┼` : Connections
- `▼ ▲` : Data flow direction
- `◄ ►` : Bidirectional communication
