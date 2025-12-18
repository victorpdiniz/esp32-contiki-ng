# WiFi UDP Communication - Sender Component (Contiki-based)

set(CONTIKI_BASE "${CMAKE_SOURCE_DIR}/../../../..")

idf_component_register(
    SRCS 
        "udp-sender.c"
        "${CONTIKI_BASE}/os/sys/autostart.c"
        "${CONTIKI_BASE}/os/sys/process.c"
        "${CONTIKI_BASE}/os/sys/timer.c"
        "${CONTIKI_BASE}/os/sys/etimer.c"
        "${CONTIKI_BASE}/arch/cpu/esp32-s3/clock.c"
        "${CONTIKI_BASE}/arch/cpu/esp32-s3/rtimer-arch.c"
        "${CONTIKI_BASE}/os/sys/rtimer.c"
        "${CONTIKI_BASE}/os/sys/ctimer.c"
        "${CONTIKI_BASE}/arch/cpu/esp32-s3/watchdog.c"
        "${CONTIKI_BASE}/os/lib/list.c"
        "${CONTIKI_BASE}/os/lib/memb.c"
        "${CONTIKI_BASE}/os/sys/node-id.c"
        "${CONTIKI_BASE}/arch/platform/esp32-s3/components/wifi-manager.c"
        "${CONTIKI_BASE}/arch/platform/esp32-s3/components/udp-manager.c"
        "${CONTIKI_BASE}/arch/platform/esp32-s3/components/freertos-bridge.c"
        "${CONTIKI_BASE}/arch/platform/esp32-s3/platform.c"
    INCLUDE_DIRS 
        "."
        "${CONTIKI_BASE}/os"
        "${CONTIKI_BASE}/os/sys"
        "${CONTIKI_BASE}/os/lib"
        "${CONTIKI_BASE}/os/dev"
        "${CONTIKI_BASE}/os/net"
        "${CONTIKI_BASE}/arch/cpu/esp32-s3"
        "${CONTIKI_BASE}/arch/platform/esp32-s3"
        "${CONTIKI_BASE}/arch/platform/esp32-s3/sys"
        "${CONTIKI_BASE}/arch/platform/esp32-s3/components"
    REQUIRES 
        nvs_flash
        esp_timer
        driver
        lwip
        esp_wifi
        esp_event
        freertos
        spi_flash
)
