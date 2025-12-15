# ESP32-S3 Contiki-NG Quick Reference

## 📦 What Was Cleaned Up

### Structure Before → After

**Platform Directory:**
- ❌ 10+ files mixed in root
- ✅ Core files + organized `components/` folder

**Examples:**
- ❌ 1 complex example with mixed docs
- ✅ 2 examples: minimal `hello-world` + advanced `iot-demo`

**Build System:**
- ❌ ESP-IDF only (`idf.py build`)
- ✅ Contiki-style Makefiles (`make build`)

**ESP-IDF Integration:**
- ❌ Unclear setup process
- ✅ Automated script + comprehensive guide

## 🚀 Quick Commands

### First-Time Setup
```bash
# 1. Install ESP-IDF
./tools/esp32-s3/setup-esp-idf.sh

# 2. Configure shell (add to ~/.bashrc)
export IDF_PATH=~/.espressif/esp-idf
alias get_idf='. $IDF_PATH/export.sh'

# 3. Activate (run in each terminal)
get_idf
```

### Build & Flash (Contiki-NG Style)
```bash
cd examples/esp32-s3/hello-world

# First time: set target
idf.py set-target esp32s3

# Build
make build

# Flash & monitor
make flash monitor
```

### Build & Flash (ESP-IDF Direct)
```bash
cd examples/esp32-s3/hello-world
idf.py set-target esp32s3
idf.py build
idf.py flash monitor
```

## 📁 File Locations

| What | Where |
|------|-------|
| **Platform core** | `arch/platform/esp32-s3/platform.c` |
| **CPU abstraction** | `arch/cpu/esp32-s3/clock.c`, `rtimer-arch.c`, `watchdog.c` |
| **WiFi/Sensor modules** | `arch/platform/esp32-s3/components/` |
| **Minimal example** | `examples/esp32-s3/hello-world/` |
| **Advanced example** | `examples/esp32-s3/iot-demo/` |
| **Setup script** | `tools/esp32-s3/setup-esp-idf.sh` |
| **Documentation** | `doc/esp32-s3-*.md` |

## 📚 Documentation Index

1. **[ESP32-S3 Structure](doc/esp32-s3-structure.md)** - Complete overview
2. **[ESP-IDF Integration](doc/esp32-s3-esp-idf-integration.md)** - External ESP-IDF setup
3. **[Platform README](arch/platform/esp32-s3/README.md)** - Platform details
4. **[Hello World](examples/esp32-s3/hello-world/README.md)** - Minimal example
5. **[IoT Demo](examples/esp32-s3/iot-demo/README.md)** - Advanced features

## 🎯 Common Tasks

### Create New Application
```bash
# Copy hello-world as template
cp -r examples/esp32-s3/hello-world examples/esp32-s3/my-app
cd examples/esp32-s3/my-app

# Edit main/hello-world.c
# Edit Makefile (change CONTIKI_PROJECT name)
# Build
make build
```

### Use Platform Components
```c
// In your application
#include "wifi-manager.h"
#include "sensor-manager.h"
#include "freertos-bridge.h"

// See iot-demo for usage examples
```

### Debug Build Issues
```bash
# Clean everything
make clean
rm -rf build

# Reconfigure
idf.py set-target esp32s3

# Rebuild
make build
```

### Check ESP-IDF Version
```bash
cd ~/.espressif/esp-idf
git describe --tags
# Should show: v5.1.2
```

## 💡 Tips

1. **Always run `get_idf` in new terminals** - activates ESP-IDF environment
2. **Start with hello-world** - simplest working example
3. **Use iot-demo as reference** - for WiFi/sensor integration
4. **Keep ESP-IDF external** - don't add to Git
5. **Check port permissions** - add user to `dialout` group if flash fails

## 🆘 Troubleshooting

| Problem | Solution |
|---------|----------|
| `idf.py: command not found` | Run `get_idf` to activate ESP-IDF |
| `Permission denied: /dev/ttyUSB0` | `sudo usermod -a -G dialout $USER` |
| `Build fails with missing IDF_PATH` | Set `export IDF_PATH=~/.espressif/esp-idf` |
| `Wrong ESP-IDF version` | Run `tools/esp32-s3/setup-esp-idf.sh` |

## 📊 Project Stats

- **Platform core**: 5 files (platform.c + configs)
- **CPU layer**: 3 files (clock, rtimer, watchdog)
- **Components**: 6 files (3 modules × 2 files each)
- **Examples**: 2 (hello-world + iot-demo)
- **Documentation**: 5 guides
- **Repository size**: ~0 MB added (ESP-IDF external)

## ✅ Summary

The ESP32-S3 platform is now:
- ✅ **Organized** - Clear separation of concerns
- ✅ **Clean** - No duplicate files
- ✅ **Documented** - Comprehensive guides
- ✅ **Contiki-style** - Familiar Makefile interface
- ✅ **Maintainable** - External ESP-IDF dependency
- ✅ **Beginner-friendly** - Simple hello-world entry point
- ✅ **Feature-complete** - Advanced iot-demo showcase
