cmake_minimum_required(VERSION 3.13)
set(target "pcf8523_rp2040")

project(${target} C CXX ASM)
add_library(${target} INTERFACE)

target_sources(${target} INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/src/pcf8523_generic.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/pcf8523_i2c_rp2040.c
)

target_include_directories(${target} INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${target} INTERFACE
	pico_stdlib
	hardware_i2c
)
