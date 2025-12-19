# WiFi UDP Sender - CMakeLists.txt

# Set ESP32-S3 as target
set(IDF_TARGET esp32s3)

cmake_minimum_required(VERSION 3.16)

# Tell CMake where to find the esp32-s3 platform component
set(EXTRA_COMPONENT_DIRS 
    "${CMAKE_CURRENT_LIST_DIR}/../../../../arch/platform/esp32-s3"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(wifi-udp-sender)
