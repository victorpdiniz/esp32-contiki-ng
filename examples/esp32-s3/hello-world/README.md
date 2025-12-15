# ESP32-S3 Hello World Example

Minimal Contiki-NG application for ESP32-S3 demonstrating basic platform features.

## What It Does

- Blinks LED on GPIO 35 every 1 second
- Prints "Hello, World" and uptime to console
- Demonstrates Contiki-NG process and etimer usage

## Hardware

- ESP32-S3 board (tested on Heltec WiFi LoRa 32 V3)
- LED on GPIO 35 (change `LED_GPIO` in `main/hello-world.c` for your board)

## Quick Start

### Method 1: Contiki-NG Style (Recommended)

```bash
# Configure target (first time only)
idf.py set-target esp32s3

# Build
make build

# Flash and monitor
make flash monitor
```

### Method 2: ESP-IDF Direct

```bash
idf.py set-target esp32s3
idf.py build
idf.py flash monitor
```

## Expected Output

```
Hello, World from ESP32-S3!
Contiki-NG is running successfully!
LED configured on GPIO 35 - SUCCESS
Tick! Uptime: 1 seconds, LED: ON
Tick! Uptime: 2 seconds, LED: OFF
Tick! Uptime: 3 seconds, LED: ON
...
```

## Project Structure

```
hello-world/
├── main/
│   ├── hello-world.c          # Application code
│   └── CMakeLists.txt          # Component build config
├── CMakeLists.txt              # Top-level ESP-IDF project
├── Makefile                    # Contiki-NG Makefile
├── Makefile.esp32-s3           # ESP32-S3 build wrappers
├── sdkconfig.defaults          # ESP-IDF default configuration
└── project-conf.h              # Contiki-NG project config
```

## Customization

### Change LED Pin

Edit `main/hello-world.c`:
```c
#define LED_GPIO  GPIO_NUM_XX  // Your GPIO number
```

### Change Blink Rate

Edit `main/hello-world.c`:
```c
etimer_set(&timer, CLOCK_SECOND * 2);  // 2 seconds instead of 1
```

## Next Steps

- **Add WiFi**: See [iot-demo](../iot-demo/) example
- **Add sensors**: Check out sensor-manager in platform components
- **Network protocols**: Explore Contiki-NG's IPv6, CoAP, MQTT support

## Troubleshooting

**LED doesn't blink**
- Check GPIO number matches your board
- Verify GPIO is not used by other peripherals

**Build fails**
- Ensure ESP-IDF is installed: `idf.py --version`
- Set target: `idf.py set-target esp32s3`
- Clean build: `make clean && make build`

**Flash fails**
- Check USB port permissions: `ls -l /dev/ttyUSB*`
- Add user to dialout group: `sudo usermod -a -G dialout $USER`
- Specify port: `idf.py -p /dev/ttyUSB0 flash`

## See Also

- [ESP32-S3 Platform](../../../arch/platform/esp32-s3/)
- [IoT Demo Example](../iot-demo/) - Advanced WiFi + sensors
- [Contiki-NG Documentation](https://docs.contiki-ng.org/)
