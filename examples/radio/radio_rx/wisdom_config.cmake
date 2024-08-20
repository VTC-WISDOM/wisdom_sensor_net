# wisdom_config.cmake
# Maintainer:
#	Evan Morse
#   emorse8686@gmail.com

# DO NOT MODIFY THE FORMATTING OF THIS LINE
# Only change the target name
set(target "radio_rx")

# Source files
list(APPEND sources
	src/radio_rx.c	
)

# Include file locations
list(APPEND includes
	src
)

# pico_stdlib included by default
list(APPEND libraries
	radio_module
)

list(APPEND definitions

)

# Only one of these should be enabled
set(stdio_uart_enable 1)
set(stdio_usb_enable 0)
