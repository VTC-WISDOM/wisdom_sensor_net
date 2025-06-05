# wisdom_config.cmake
# Maintainer:
#	Evan Morse
#   emorse8686@gmail.com

# DO NOT MODIFY THE FORMATTING OF THIS LINE
# Only change the target name
set(target "rtc_sync_client")

# Source files
list(APPEND sources
	src/client.c	
)

# Include file locations
list(APPEND includes
	src
)

# pico_stdlib included by default
list(APPEND libraries
	pcf8523_rp2040
	hardware_i2c
)

list(APPEND definitions
)

# Only one of these should be enabled
set(stdio_uart_enable 0)
set(stdio_usb_enable 1)
