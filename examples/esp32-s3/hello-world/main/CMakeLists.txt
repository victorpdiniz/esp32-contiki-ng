set(CONTIKI_BASE "${CMAKE_SOURCE_DIR}/../../..")

idf_component_register(
    SRCS 
        "hello-world.c"
        "${CONTIKI_BASE}/os/sys/autostart.c"
    INCLUDE_DIRS 
        "${CONTIKI_BASE}/os"
        "${CONTIKI_BASE}/os/sys"
        "${CONTIKI_BASE}/os/dev"
        "${CONTIKI_BASE}/arch/platform/esp32-s3"
        "${CONTIKI_BASE}/arch/platform/esp32-s3/components"
        "${CONTIKI_BASE}/arch/cpu/esp32-s3"
    REQUIRES 
        esp32-s3
)
