set(CONTIKI_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../..")

idf_component_register(
    SRCS 
        "hello-world.c"
        "${CONTIKI_ROOT}/os/sys/autostart.c"
    INCLUDE_DIRS 
        "."
        "${CONTIKI_ROOT}/arch/platform/esp32-s3"
        "${CONTIKI_ROOT}/arch/cpu/esp32-s3"
        "${CONTIKI_ROOT}/os"
        "${CONTIKI_ROOT}/os/sys"
        "${CONTIKI_ROOT}/os/lib"
    REQUIRES 
        esp32-s3
        nvs_flash
        esp_timer
)
