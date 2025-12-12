# Hello World for ESP32-S3

This is a minimal example demonstrating Contiki-NG running on the ESP32-S3 platform.

## Features

- Prints "Hello, World from ESP32-S3!" at startup
- Uses etimer to print a message every second
- Shows uptime in seconds

## Building

First, set up the ESP-IDF environment:

```bash
source ~/esp/v5.5.1/esp-idf/export.sh
```

Then build the example:

```bash
make TARGET=esp32-s3
```

## Flashing

To flash to your ESP32-S3 board:

```bash
make TARGET=esp32-s3 upload ESPTOOL_PORT=/dev/ttyUSB0
```

Replace `/dev/ttyUSB0` with your actual serial port.

## Monitoring

To see the output:

```bash
make TARGET=esp32-s3 monitor
```

Or use any serial terminal at 115200 baud.

## Expected Output

```
ESP32-S3 Platform Init Stage One
ESP32-S3 with 2 CPU cores, WiFi/BLE, silicon revision 0, 8MB embedded flash
ESP32-S3 Platform Init Stage Three
Contiki-NG initialized. Entering main loop.
Hello, World from ESP32-S3!
Contiki-NG is running successfully.
Tick! Uptime: 1 seconds
Tick! Uptime: 2 seconds
Tick! Uptime: 3 seconds
...
```

## Notes

- This is a minimal port focusing on getting Contiki-NG's core running
- Networking support is not yet implemented
- Radio drivers are not yet included
- For production use, additional peripherals and features need to be added
