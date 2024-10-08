set(target "example_rudp_rx")

# Source files
list(APPEND sources
	src/rudp_rx.c
)

# Include file locations
list(APPEND includes
	src
)

# pico_stdlib included by default
list(APPEND libraries
	rfm69_pico
)

list(APPEND definitions
	# RFM69 pin definitions
	RFM69_PIN_MISO=16	
	RFM69_PIN_MOSI=19
	RFM69_PIN_CS=17
	RFM69_PIN_SCK=18
	RFM69_PIN_RST=20
)

# Only one of these should be enabled
set(stdio_uart_enable 0)
set(stdio_usb_enable 1)
