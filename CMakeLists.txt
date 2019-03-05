# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(volkhin_nixie_timer)
set(COMPONENTS "btstack" "alexa_gadgets_toolkit" "spi_master" "tft")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
