cmake_minimum_required(VERSION 3.13)

project(sht30_rp2040 C CXX ASM)

add_library(sht30_rp2040 INTERFACE)

target_sources(sht30_rp2040 INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/src/sht30_rp2040.c
)

target_include_directories(sht30_rp2040 INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(sht30_rp2040 INTERFACE
	pico_stdlib
	hardware_i2c
)
