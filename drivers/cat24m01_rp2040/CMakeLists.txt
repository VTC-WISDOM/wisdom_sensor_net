cmake_minimum_required(VERSION 3.13)

project(cat24m01_rp2040 C CXX ASM)

add_library(cat24m01_rp2040 INTERFACE)

target_sources(cat24m01_rp2040 INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/src/cat24m01_rp2040.c
)

target_include_directories(cat24m01_rp2040 INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(cat24m01_rp2040 INTERFACE
	pico_stdlib
	hardware_i2c
)
