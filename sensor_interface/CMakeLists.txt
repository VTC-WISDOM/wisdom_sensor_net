cmake_minimum_required(VERSION 3.13)

set(WISDOM_PROJECT_PATH "~/pico/wisdom_sensor_net")
get_filename_component(WISDOM_PROJECT_PATH "${WISDOM_PROJECT_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
set(WISDOM_PROJECT_PATH ${WISDOM_PROJECT_PATH} CACHE PATH "Root of Wisdom Repo" FORCE)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# SHT30
message("wisdom_init: initializing SHT30 sensor interface")
add_subdirectory(${WISDOM_PROJECT_PATH}/drivers/sht30_rp2040)

set(target "wisdom_sensor_interface")
project(${target} C CXX ASM)

add_library(${target} INTERFACE)

target_sources(${target} INTERFACE

	# Base template
	src/wisdom_sensors.c
	src/sensor_interface.c

	# Test sensor
	src/sensors/test/test_sensor.c

	# SHT30 (temp/humidity sensor)
	src/sensors/sht30/sht30_wsi.c

)

target_compile_definitions(${target} INTERFACE

	# SHT30 (temp/humidity sensor)
	SHT30_I2C=i2c0,
	SHT30_SCL=5,
	SHT30_SDA=4
)

# Add the standard include files to the build
target_include_directories(${target} INTERFACE
	src	
)

target_link_libraries(${target} INTERFACE
	sht30_rp2040
)
