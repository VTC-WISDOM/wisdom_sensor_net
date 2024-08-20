set(WISDOM_PROJECT_PATH "~/pico/wisdom_sensor_net")
get_filename_component(WISDOM_PROJECT_PATH "${WISDOM_PROJECT_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")

set(WISDOM_PROJECT_PATH ${WISDOM_PROJECT_PATH} CACHE PATH "Root of Wisdom Repo" FORCE)
set(WISDOM_DRIVERS_PATH "${WISDOM_PROJECT_PATH}/drivers")

# RFM69
message("wisdom_init: building rfm69_pico library")
add_subdirectory(${WISDOM_DRIVERS_PATH}/rfm69_rp2040 rfm69_rp2040)

# FatFs_SPI
message("wisdom_init: building FatFs_SPI library")
add_subdirectory(${WISDOM_DRIVERS_PATH}/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI FatFs_SPI)

# Load local config
message("wisdom_init: loading local wisdom_config.cmake file")
include(wisdom_config.cmake)
