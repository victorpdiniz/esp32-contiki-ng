# ESP32-S3 Platform for Contiki-NG

This directory contains the platform-specific code for running Contiki-NG on ESP32-S3 microcontrollers.

## Overview

The ESP32-S3 port uses the ESP-IDF (Espressif IoT Development Framework) as a Hardware Abstraction Layer (HAL) for low-level hardware access, while running Contiki-NG's native kernel and process scheduler (not FreeRTOS).

## Architecture

```
Contiki-NG Kernel & Processes
         ↓
    ESP-IDF HAL (hardware access only)
         ↓
     ESP32-S3 Hardware
```

### Key Features

- **Native Contiki-NG kernel** - Uses Contiki's event-driven process model
- **No FreeRTOS** - Direct hardware control via ESP-IDF HAL
- **Hardware timer-based clock** - Uses Timer Group 0 for system clock
- **Low-power idle** - Uses WAITI instruction during idle periods
- **Hardware watchdog** - RTC watchdog for system monitoring

## Directory Structure

```
arch/platform/esp32-s3/
├── platform.c           # Platform initialization and main loop
├── contiki-conf.h       # Contiki configuration
├── esp32-s3-conf.h      # Platform configuration
├── esp32-s3-def.h       # Platform definitions
├── esp32s3.ld           # Linker script
├── Makefile.esp32-s3    # Build configuration
└── README.md

arch/cpu/esp32-s3/
├── clock.c              # System clock implementation
├── rtimer-arch.c        # High-resolution timer
├── rtimer-arch.h        # Rtimer header
├── watchdog.c           # Watchdog implementation
├── startup.c            # CPU startup code
└── Makefile.esp32-s3    # CPU build configuration
```

## Prerequisites

### ESP-IDF Installation

This port requires ESP-IDF v5.5.1 or later:

```bash
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
git checkout v5.5.1
./install.sh esp32s3
```

### Toolchain Setup

Before building, activate the ESP-IDF environment:

```bash
source ~/esp/v5.5.1/esp-idf/export.sh
```

## Building

Navigate to an example directory and build:

```bash
cd examples/esp32-s3/hello-world
make TARGET=esp32-s3
```

## Flashing

To flash the compiled firmware:

```bash
make TARGET=esp32-s3 upload ESPTOOL_PORT=/dev/ttyUSB0
```

## Hardware Support

### Implemented

- [x] System clock (128 Hz)
- [x] High-resolution timer (rtimer)
- [x] Watchdog timer
- [x] UART console output
- [x] NVS (Non-Volatile Storage)

### Not Yet Implemented

- [ ] GPIO
- [ ] LEDs
- [ ] Buttons
- [ ] WiFi
- [ ] Bluetooth
- [ ] Radio drivers
- [ ] Networking stack integration
- [ ] Power management (deep sleep, light sleep)
- [ ] SPI, I2C, ADC, etc.

## Configuration

Platform-specific configuration can be adjusted in:

- `esp32-s3-conf.h` - Platform settings
- `contiki-conf.h` - Contiki-NG settings
- Project-specific `project-conf.h` - Application settings

### Key Configuration Options

```c
/* System clock frequency */
#define CLOCK_CONF_SECOND 128

/* UART console */
#define CONFIG_ESP_CONSOLE_UART_NUM 0
#define CONFIG_ESP_CONSOLE_UART_BAUDRATE 115200

/* Watchdog timeout */
#define WATCHDOG_TIMEOUT_MS 5000
```

## Memory Layout

- **IRAM**: 64 KB for time-critical code (ISRs, etc.)
- **DRAM**: 480 KB for data and BSS
- **Flash**: 4 MB for code and read-only data
- **RTC RAM**: 8 KB for deep sleep retention

## Limitations

This is an initial minimal port. Current limitations:

1. No networking support yet
2. No radio drivers
3. No power management
4. Minimal peripheral support
5. Polling-based rtimer (not interrupt-based)

## Future Work

- Implement networking stack integration
- Add WiFi and BLE support
- Implement radio drivers for 6LoWPAN/Thread
- Add power management (light sleep, deep sleep)
- Add peripheral drivers (GPIO, SPI, I2C, ADC)
- Optimize memory usage
- Add interrupt-based rtimer scheduling

## Contributing

Contributions are welcome! Areas that need work:

- Networking and radio drivers
- Power management
- Peripheral drivers
- Documentation improvements
- Testing and bug fixes

## License

This port is released under the same 3-clause BSD license as Contiki-NG.

## References

- [ESP32-S3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)
- [Contiki-NG Documentation](https://docs.contiki-ng.org/)
