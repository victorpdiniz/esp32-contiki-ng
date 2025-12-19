# hello-world

A minimal Contiki-NG example for ESP32-S3, blinking an LED every second.

## Build and Run

```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Output

```
Hello, world from ESP32-S3!
LED toggled! State: 1, Uptime: 1 seconds
LED toggled! State: 0, Uptime: 2 seconds
...
```
