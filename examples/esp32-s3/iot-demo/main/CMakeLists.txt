set(CONTIKI_BASE "${CMAKE_SOURCE_DIR}/../../..")
set(PLATFORM_DIR "${CONTIKI_BASE}/arch/platform/esp32-s3")

idf_component_register(
    SRCS 
        "iot-app.c"
        "${CONTIKI_BASE}/os/sys/autostart.c"
        "${PLATFORM_DIR}/components/wifi-manager.c"
        "${PLATFORM_DIR}/components/udp-manager.c"
    INCLUDE_DIRS 
        "${PLATFORM_DIR}"
        "${PLATFORM_DIR}/components"
        "${CONTIKI_BASE}/arch/cpu/esp32-s3"
        "${CONTIKI_BASE}/os"
        "${CONTIKI_BASE}/os/sys"
        "${CONTIKI_BASE}/os/lib"
    REQUIRES 
        esp32-s3
        nvs_flash
        esp_timer
        driver
)
