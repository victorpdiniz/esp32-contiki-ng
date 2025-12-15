CONTIKI_PROJECT = iot-demo
all: $(CONTIKI_PROJECT)

CONTIKI = ../../..

# Include the ESP32-S3 platform
PLATFORMS_ONLY = esp32-s3

# Optional: Node ID
MAKE_MAC = MAKE_MAC_FROM_ID
MAKE_NET = MAKE_NET_FROM_ID

# Include platform-specific Makefile for ESP32-S3 build wrappers
include Makefile.esp32-s3
