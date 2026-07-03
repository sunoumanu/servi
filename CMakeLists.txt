cmake_minimum_required(VERSION 3.16)

# Set the target chip explicitly to ESP32-C6
set(CONFIG_IDF_TARGET "esp32c6")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32c6-bootstrap)
