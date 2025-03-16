//  rfm69_dio_rx
//  2025.02.04

//	Copyright (C) 2024 
//	Evan Morse
//	Amelia Vlahogiannis
//	Noelle Steil
//	Jordan Allen
//	Sam Cowan
//	Rachel Cleminson

//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.

//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.

//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.

// rfm69 packet rx/tx example utilizing gio interrupts.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "tusb.h"

#include "rp2x_rfm69_rudp.h"

static const char *gpio_irq_str[] = {
        "LEVEL_LOW",  // 0x1
        "LEVEL_HIGH", // 0x2
        "EDGE_FALL",  // 0x4
        "EDGE_RISE"   // 0x8
};

static char event_str[128];
void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}

bool payload_ready = false;
void callback(uint gpio, uint32_t events) {
    //gpio_event_string(event_str, events);
    //printf("GPIO %d %u %s\n", gpio, events, event_str);
	payload_ready = true;
};

void main() {
	stdio_init_all();
	while (!tud_cdc_connected()) { sleep_ms(100); };

	// SPI init
    spi_init(RFM69_SPI, 1000*1000);
    gpio_set_function(RFM69_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(RFM69_PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(RFM69_PIN_MOSI, GPIO_FUNC_SPI);

	// Drive CS pin high
    gpio_init(RFM69_PIN_CS);
    gpio_set_dir(RFM69_PIN_CS, GPIO_OUT);
    gpio_put(RFM69_PIN_CS, 1);

	struct rfm69_config_s config = {
		.spi      = RFM69_SPI,
		.pin_cs   = RFM69_PIN_CS,
		.pin_rst  = RFM69_PIN_RST,
		.pin_dio0 = RFM69_PIN_DIO0,
		.pin_dio1 = RFM69_PIN_DIO1,
		.pin_dio2 = RFM69_PIN_DIO2
	};

	rfm69_context_t rfm;
	if (rfm69_rudp_init(&rfm, &config) == false)
		goto ERROR_LOOP;

	rfm69_node_address_set(&rfm, 0x01);
#define PAYLOAD_BUFFER_SIZE (VP_RX_BUFFER_MAX)
	for (;;) {
		uint8_t payload[PAYLOAD_BUFFER_SIZE] = {0};

		VP_RX_ERROR_T rval = rfm69_rx_variable_packet(
				&rfm, payload, PAYLOAD_BUFFER_SIZE, 10000);

		if (rval == VP_RX_OK || rval == VP_RX_CRC_FAILURE) {
			if (rval == VP_RX_CRC_FAILURE)
				printf("CRC failed!\n");

			printf("received: %u\n", payload[0]);
			printf("address: %u\n", payload[1]);
			printf("last: %u\n", payload[payload[0]]);
		} else {
			printf("error: %i\n", rval);
		}
	}

	// Loop forever with error
ERROR_LOOP:
	for (;;) {
		printf("Radio failed to initialize.\n");
		sleep_ms(3000);
	}
}
