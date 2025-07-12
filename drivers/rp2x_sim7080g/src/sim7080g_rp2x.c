#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "sim7080g_rp2x.h"

void sim7080g_init(sim7080g_context_t *context, const struct sim7080g_config config) {
	context->uart_index = config.uart_index;
	context->pin_pwr = config.pin_pwr;

	gpio_init(context->pin_pwr);
	gpio_set_dir(context->pin_pwr, GPIO_OUT);
	gpio_put(context->pin_pwr, 1);
}

static bool _pwr_switch(sim7080g_context_t *context) {
	bool level = gpio_get(context->pin_pwr);
	gpio_put(context->pin_pwr, !level);
	return level;
}

void sim7080g_pwr_toggle(sim7080g_context_t *context) {
	_pwr_switch(context);
	sleep_ms(1100);
	_pwr_switch(context);
}

void sim7080g_pwr_set(sim7080g_context_t *context, bool level) {
	gpio_put(context->pin_pwr, level);}

uint sim7080g_uart_read(sim7080g_context_t *context, void *buffer, size_t size) {
	uint count = 0;
	uint8_t c;
	while(uart_is_readable(UART_INSTANCE(context->uart_index)) && count < size) {
		uart_read_blocking(UART_INSTANCE(context->uart_index), &c, 1);	

		// Sometimes garbage bytes come through when uart starts up
		// so we have to filter those out.
		// However, we still want to pass on \r and \n so we check
		// for those.
		if (!isprint(c))
			if (c != '\r' && c != '\n')	
				continue;

		*(char *)buffer++ = c;

		count++;
	}

	return count;
}

uint sim7080g_uart_write(sim7080g_context_t *context, const void *buffer, size_t size) {
	uint count = 0;
	while(uart_is_writable(UART_INSTANCE(context->uart_index)) && count < size) {
		uart_write_blocking(UART_INSTANCE(context->uart_index), buffer++, 1);	
		count++;
	}

	return count;
}

void sim7080g_uart_write_all(sim7080g_context_t *context, const void *buffer, size_t size) {
	uint count = 0;
	while (size) {
		count = sim7080g_uart_write(context, buffer, size);
		size -= count;
		buffer = (uint8_t *)buffer + count;
	}
}

// HELPER FUNCTIONS FOR SENDING AT COMMANDS.
// THEY DO NOT CHECK FOR RETURN.
// TODO: Create blocking (timeout?) functions for choosing to wait 
//       for a response.

void sim7080g_cn_state(sim7080g_context_t *context) {
	const char *str = AT_PREFIX;
	sim7080g_uart_write(context, str, strlen(str));		

	str = ATC_CNACT;
	sim7080g_uart_write(context, str, strlen(str));		

	sim7080g_uart_write(context, "?\r", 2);		
}

void sim7080g_cn_activate(sim7080g_context_t *context, uint cn_index, bool activate) {
	const char *str = AT_PREFIX;
	sim7080g_uart_write(context, str, strlen(str));		

	str = ATC_CNACT;
	sim7080g_uart_write(context, str, strlen(str));		

	char format[6];
	sprintf(format, "=%u,%c\r", cn_index, activate ? '1' : '0');
	sim7080g_uart_write(context, format, 5);		
}
