# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "~/pico/pico-sdk")

set(PICO_BOARD adafruit_kb2040 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(turbidity C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(turbidity turbidity.c ../pico-ssd1306/ssd1306.c)

pico_set_program_name(turbidity "turbidity")
pico_set_program_version(turbidity "0.1")

pico_enable_stdio_uart(turbidity 1)
pico_enable_stdio_usb(turbidity 0)

# Add the standard library to the build
target_link_libraries(turbidity
        pico_stdlib
	hardware_i2c
	hardware_gpio
	hardware_adc
)

# Add the standard include files to the build
target_include_directories(turbidity PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
  ../pico-ssd1306/
)

# Add any user requested libraries
target_link_libraries(turbidity 
        hardware_i2c
	hardware_gpio
	hardware_adc
        )

pico_add_extra_outputs(turbidity)

