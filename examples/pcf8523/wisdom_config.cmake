# wisdom_config.cmake
# Maintainer:
#	Evan Morse
#   emorse8686@gmail.com

# DO NOT MODIFY THE FORMATTING OF THIS LINE
# Only change the target name
set(target "pcf8523_example")

# Source files
list(APPEND sources
	src/pcf8523_main.c	
)

# Include file locations
list(APPEND includes
	src
)

# pico_stdlib included by default
list(APPEND libraries
	pcf8523_rp2040
)

list(APPEND definitions

)

# Only one of these should be enabled
set(stdio_uart_enable 0)
set(stdio_usb_enable 1)
