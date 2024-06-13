# wisdom_config.cmake
# Maintainer:
#	Evan Morse
#   emorse8686@gmail.com

# DO NOT MODIFY THE FORMATTING OF THIS LINE
# Only change the target name
set(target "sht30_example")

# Source files
list(APPEND sources
	src/sht30_main.c	
	
)

# Include file locations
list(APPEND includes
	./src
)

# pico_stdlib included by default
list(APPEND libraries
	hardware_i2c
)

list(APPEND definitions
	## [rfm69_pico] (Radio)

	# Enables high power routines for RFM69
	#RFM69_HIGH_POWER

	# RFM69 pin definitions 
	#RFM69_SPI=spi0
	#RFM69_PIN_MISO=16	
	#RFM69_PIN_MOSI=19
	#RFM69_PIN_CS=17
	#RFM69_PIN_SCK=18
	#RFM69_PIN_RST=20

	## [FatFs_SPI] (SD Card)

	# SD pin definitions
	#SD_SPI=spi0
	#SD_PIN_MISO=16
	#SD_PIN_MOSI=19
	#SD_PIN_CS=22
	#SD_PIN_SCK=18
)

# Only one of these should be enabled
set(stdio_uart_enable 0)
set(stdio_usb_enable 1)
