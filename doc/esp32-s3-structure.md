# ESP32-S3 Platform Structure

This document explains the cleaned-up ESP32-S3 platform organization in Contiki-NG.

## 📁 Directory Structure

```
contiki-ng/
├── arch/
│   ├── platform/esp32-s3/           # Platform-specific code
│   │   ├── platform.c                # Platform initialization
│   │   ├── contiki-conf.h            # Platform configuration
│   │   ├── esp32-s3-conf.h           # Hardware definitions
│   │   ├── esp32s3.ld                # Linker script
│   │   ├── CMakeLists.txt            # ESP-IDF component
│   │   └── components/               # Optional platform modules ⭐
│   │       ├── freertos-bridge.c/h   # FreeRTOS↔Contiki bridge
│   │       ├── wifi-manager.c/h      # WiFi management
│   │       └── sensor-manager.c/h    # Sensor framework
│   │
│   └── cpu/esp32-s3/                 # CPU abstraction layer
│       ├── clock.c                   # Contiki clock implementation
│       ├── rtimer-arch.c/h           # Real-time timer
│       └── watchdog.c                # Watchdog timer
│
├── examples/esp32-s3/
│   ├── hello-world/                  # ⭐ Minimal example
│   │   ├── main/hello-world.c        # Simple LED blink app
│   │   ├── Makefile                  # Contiki-style build
│   │   └── Makefile.esp32-s3         # ESP-IDF wrappers
│   │
│   └── iot-demo/                     # ⭐ Advanced example
│       ├── main/iot-app.c            # WiFi + sensors + multi-task
│       ├── ARCHITECTURE.md           # System design docs
│       ├── Makefile                  # Contiki-style build
│       └── ...
│
├── tools/esp32-s3/
│   └── setup-esp-idf.sh              # ⭐ ESP-IDF installer script
│
└── doc/
    └── esp32-s3-esp-idf-integration.md  # ⭐ ESP-IDF integration guide
```

## 🎯 Key Improvements

### 1. **Organized Platform Components**

**Before:**
```
arch/platform/esp32-s3/
├── platform.c
├── freertos-bridge.c      ❌ Cluttered
├── freertos-bridge.h
├── wifi-manager.c
├── wifi-manager.h
├── sensor-manager.c
└── sensor-manager.h
```

**After:**
```
arch/platform/esp32-s3/
├── platform.c              ✅ Core platform files only
├── contiki-conf.h
├── esp32-s3-conf.h
└── components/             ✅ Optional modules grouped
    ├── freertos-bridge.c/h
    ├── wifi-manager.c/h
    └── sensor-manager.c/h
```

### 2. **Separated Examples by Complexity**

| Example | Description | Use Case |
|---------|-------------|----------|
| **hello-world** | Minimal LED blink | Learning Contiki-NG basics |
| **iot-demo** | WiFi + sensors + tasks | Real IoT applications |

### 3. **Contiki-NG Style Build System**

Users can now use familiar Contiki-NG commands:

```bash
cd examples/esp32-s3/hello-world
make build          # Instead of: idf.py build
make flash          # Instead of: idf.py flash
make monitor        # Instead of: idf.py monitor
```

The Makefile wraps ESP-IDF commands while maintaining Contiki-NG conventions.

### 4. **External ESP-IDF (Best Practice)**

ESP-IDF is **NOT** included in the repository. Instead:
- Standard installation at `~/.espressif/esp-idf`
- Automated setup script: `tools/esp32-s3/setup-esp-idf.sh`
- Docker support for CI/CD
- Version pinned to v5.1.2

**Benefits:**
- Repository stays lightweight (~0 MB vs +500 MB)
- Easy updates
- Standard workflow
- CI/CD friendly

## 🚀 Quick Start

### 1. Install ESP-IDF (One Time)

```bash
./tools/esp32-s3/setup-esp-idf.sh
```

### 2. Configure Environment

Add to `~/.bashrc`:
```bash
export IDF_PATH=~/.espressif/esp-idf
alias get_idf='. $IDF_PATH/export.sh'
```

Then:
```bash
source ~/.bashrc
get_idf
```

### 3. Build Examples

**Minimal Hello World:**
```bash
cd examples/esp32-s3/hello-world
idf.py set-target esp32s3  # First time only
make build
make flash monitor
```

**Advanced IoT Demo:**
```bash
cd examples/esp32-s3/iot-demo
idf.py set-target esp32s3  # First time only
make build
make flash monitor
```

## 📚 Documentation

- **Platform Guide**: [arch/platform/esp32-s3/README.md](../arch/platform/esp32-s3/README.md)
- **Hello World**: [examples/esp32-s3/hello-world/README.md](../examples/esp32-s3/hello-world/README.md)
- **IoT Demo**: [examples/esp32-s3/iot-demo/README.md](../examples/esp32-s3/iot-demo/README.md)
- **ESP-IDF Integration**: [doc/esp32-s3-esp-idf-integration.md](esp32-s3-esp-idf-integration.md)

## 🔧 Component Usage

Platform components are **optional** and compiled into the platform library. Applications can use them:

```c
#include "freertos-bridge.h"  // FreeRTOS communication
#include "wifi-manager.h"     // WiFi connectivity
#include "sensor-manager.h"   // Sensor framework
```

**Note:** These are **NOT** required for minimal applications (see hello-world).

## 🏗️ Architecture

```
┌─────────────────────────────────┐
│   Application (iot-app.c)      │  ← Your code
├─────────────────────────────────┤
│  Platform Components (optional) │  ← WiFi, Sensors, Bridge
│  ├─ wifi-manager               │
│  ├─ sensor-manager             │
│  └─ freertos-bridge            │
├─────────────────────────────────┤
│  Platform Core (platform.c)    │  ← Contiki initialization
├─────────────────────────────────┤
│  CPU Layer (clock, rtimer)     │  ← Hardware abstraction
├─────────────────────────────────┤
│  ESP-IDF (FreeRTOS, HAL)       │  ← Hardware drivers
└─────────────────────────────────┘
```

## 🎓 Next Steps

1. ✅ Run hello-world to verify setup
2. ✅ Study iot-demo for advanced features
3. 📖 Read [ESP-IDF Integration Guide](esp32-s3-esp-idf-integration.md)
4. 🔨 Build your own application
5. 🌐 Add networking (IPv6, CoAP, MQTT)

## ❓ FAQ

**Q: Why isn't ESP-IDF in the repo?**  
A: Following standard practice - keeps repo lightweight, allows independent updates. See [ESP-IDF Integration Guide](esp32-s3-esp-idf-integration.md).

**Q: What's the difference between platform/ and cpu/?**  
A: `cpu/` = hardware abstraction (clock, timers), `platform/` = board-specific init + optional features.

**Q: Can I use just Contiki without WiFi/sensors?**  
A: Yes! See hello-world - it only uses core Contiki (process, etimer, GPIO).

**Q: How do I add my own component?**  
A: Add files to `arch/platform/esp32-s3/components/` and update platform CMakeLists.txt.

## 📝 Summary of Changes

| What | Before | After |
|------|--------|-------|
| **Platform files** | 10+ files in root | Core files + components/ |
| **Examples** | 1 complex example | 2 examples (simple + advanced) |
| **Build** | ESP-IDF only | Makefile wrappers |
| **ESP-IDF** | Manual setup | Automated script |
| **Docs** | Mixed in example | Organized by topic |

## 🤝 Contributing

When adding ESP32-S3 features:
1. Core platform changes → `arch/platform/esp32-s3/`
2. Optional modules → `arch/platform/esp32-s3/components/`
3. Simple examples → Similar to hello-world
4. Complex demos → Create new folder like iot-demo
5. Update this guide and component READMEs
