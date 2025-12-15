set(CONTIKI_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../..")
set(PLATFORM_DIR "${CONTIKI_ROOT}/arch/platform/esp32-s3")

idf_component_register(
    SRCS 
        "iot-app.c"
        "${CONTIKI_ROOT}/os/sys/autostart.c"
    INCLUDE_DIRS 
        "."
        "${PLATFORM_DIR}"
        "${PLATFORM_DIR}/components"
        "${CONTIKI_ROOT}/arch/cpu/esp32-s3"
        "${CONTIKI_ROOT}/os"
        "${CONTIKI_ROOT}/os/sys"
        "${CONTIKI_ROOT}/os/lib"
    REQUIRES 
        esp32-s3
        nvs_flash
        esp_timer
        driver
)
