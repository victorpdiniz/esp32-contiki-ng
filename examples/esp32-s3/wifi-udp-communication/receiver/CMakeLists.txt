# WiFi UDP Receiver - CMakeLists.txt

# Set ESP32-S3 as target
set(IDF_TARGET esp32s3)

cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(wifi-udp-receiver)
