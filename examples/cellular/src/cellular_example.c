// cellular_example.c

//	Copyright (C) 2024 
//	Evan Morse
//	Amelia Vlahogiannis
//	Sam Cowan
//	Marin Bove

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

#include <stdio.h>
#include <ctype.h>

#include "pico/stdlib.h"
#include "tusb.h"
#include "hardware/gpio.h"

#include "whale.h"
#include "rp2x_sim7080g.h"



const char sim_apn[] = "iot.1nce.net";
const char server_url[] = "amelia.wisdomresearch.net";
const uint port = 8086;
const char payload[] = "meow!";
uint8_t    buf[64] = {0};

void main() {
	stdio_init_all();

	while (!tud_cdc_connected()) { sleep_ms(100); };
	
	static sim7080g_inst_t sim = {
		.uart = uart0,
		.uart_pin_tx = 0,
		.uart_pin_rx = 1,
		.pin_pwr = 14
	};

	//sim7080g_init(sim);

	printf("starting sim7080g... ");
	sim7080g_init(sim);
	if(!sim7080g_start(sim)) goto ERROR_LOOP;
	printf("started\n");

	sleep_ms(2000);
	printf("power off test\n");
	if(!sim7080g_power_off(sim)) printf("power off error\n");

	sleep_ms(2500);
	printf("power on test\n");
	sim7080g_power_on(sim);
	

	sleep_ms(3000);
	printf("\nstarting loop\n");

	for(int loops = 0;; loops++) {

	printf("\n\nloop %i\n", loops);


	printf("sending command... ");
	if(sim7080g_send_at_command(sim, "+GMM;I"))
	                            printf("success\n");
	else printf("failed to send command\n");

	printf("reading back response... \n");
	if(sim7080g_get_response(sim, buf, sizeof(buf))){
		
		for(int i = 0; i < strlen(buf); i++){
			if(isprint(buf[i])) printf("%c", buf[i]);
			else if(buf[i] == '\r') printf("\\r");
			else if(buf[i] == '\n') printf("\\n");
			else printf("\'0x%02x\'", buf[i]);
		}
	} else printf("failed to read response\n");

	sleep_ms(2000);
	}
	

ERROR_LOOP:
	for(;;) {
		printf("error!");
		sleep_ms(500);
	}

}
