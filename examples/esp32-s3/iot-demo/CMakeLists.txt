# ESP-IDF Project CMakeLists for Contiki-NG on ESP32-S3

cmake_minimum_required(VERSION 3.16)

# Set target to ESP32-S3
set(IDF_TARGET esp32s3)

# Set the components to include the platform directory
set(EXTRA_COMPONENT_DIRS 
    "${CMAKE_CURRENT_LIST_DIR}/../../../arch/platform/esp32-s3"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(contiki-ng-esp32s3)
