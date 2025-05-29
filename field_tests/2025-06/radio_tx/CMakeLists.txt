# Generated Cmake Pico project file
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (NOT DEFINED ENV{PICO_SDK_PATH})
	set(PICO_SDK_PATH "~/pico/pico-sdk")
else()
	set(PICO_SDK_PATH "$ENV{PICO_SDK_PATH}")
endif()

if (NOT DEFINED ENV{WHALE_PATH})
	set(WHALE_PATH "~/wisdom/whale")
else()
	set(WHALE_PATH "$ENV{WHALE_PATH}")
endif()

get_filename_component(WHALE_PATH "${WHALE_PATH}" REALPATH)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(external/pico_sdk_import.cmake)
# Import Wisdom HAL Extension
include(external/whale_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "2.0.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.0.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(target "radio_example_tx")
project(${target} C CXX ASM)
add_executable(${target})

pico_sdk_init()

# Add the pico standard library to the build
target_link_libraries(${target} pico_stdlib)
# Add wisdom hal extension
target_link_libraries(${target} wisdom_hal_extension)

pico_enable_stdio_uart(${target} 0)
pico_enable_stdio_usb(${target} 1)

target_sources(${target} PRIVATE src/radio_tx.c)

target_include_directories(${target} PRIVATE src)

pico_add_extra_outputs(${target})
