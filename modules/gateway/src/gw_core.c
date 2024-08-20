#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "sim7080g_pico.h"
#include "cbuffer.h"

#include "gw_core_error.h"
#include "gateway_codes.h"
#include "gateway_queue.h"

#define UART_BAUD 115200

#define COMMAND_DISPATCHED (0)
#define COMMAND_INCOMPLETE (1)
#define COMMAND_BUFFER_FULL (-1)
#define COMMAND_UNRECOGNIZED (-2)

#define SERVER_IP "34.69.199.172"

typedef enum _modem_state {
	MODEM_UNINITIALIZED,
	MODEM_STOPPED,
	MODEM_STARTED,
	MODEM_CN_ACTIVE,
	MODEM_SERVER_CONNECTED
} MODEM_STATE_T;

static sim7080g_context_t *_gateway = NULL;
static MODEM_STATE_T _modem_state = MODEM_UNINITIALIZED;
static bool _modem_power_toggled = false;

// Command flags
static bool _start_command_issued = false;

// Data output buffer
#define MODEM_BUFFER_OUT_SIZE (1024 * 10) // 10 KB
static cbuffer_t *_modem_buffer_out = NULL;

// Command buffer
// Should only need to hold several commands at a time
#define MODEM_BUFFER_COMMAND_SIZE (sizeof (uint32_t) * 100)
static cbuffer_t *_modem_buffer_command = NULL;

// Internal func prototypes
static bool _modem_cn_available(void);
static int _message_queue_process(void);
static void _dispatch_message_buffer(uint32_t buffer);
static int _build_command(uint32_t buffer);
static bool _command_buffer_push(void *command, uint size);
static void _command_buffer_execute(void);

uint16_t htons(uint16_t num) {
	uint16_t output = 0;
	for (int i = 0; i < 16; i += 8)
		output |= ((num >> i) & 0xFF) << (16 - 8 - i);	
	return output;
};

uint32_t htonl(uint32_t num) {
	uint32_t output = 0;
	for (int i = 0; i < 32; i += 8)
		output |= ((num >> i) & 0xFF) << (32 - 8 - i);	
	return output;
};

static void gw_core_panic(GW_CORE_ERROR_T error) {
	gw_core_error_set(error);
	for (;;) {
		printf("%s\n", gw_core_error_str);
		sleep_ms(3000);
	}
}

void gw_core_entry(void) {

	bool success = false;
	char *error = "ok";

	_modem_buffer_out = cbuffer_create(MODEM_BUFFER_OUT_SIZE);
	_modem_buffer_command = cbuffer_create(MODEM_BUFFER_COMMAND_SIZE);

	if (!_modem_buffer_out || !_modem_buffer_command) {
		error = "cbuffer_create returned NULL";
		goto LOOP_BEGIN;
	}

	_gateway = sim7080g_create();
	if (_gateway == NULL) {
		error = "sim7080g_create returned NULL";
		goto LOOP_BEGIN;
	}

	sim7080g_init(
			_gateway,
			GATEWAY_APN,
			GATEWAY_UART,
			GATEWAY_PIN_TX,
			GATEWAY_PIN_RX,
			GATEWAY_PIN_PWR
	);

	_modem_state = MODEM_STOPPED;

	success = true;
LOOP_BEGIN:
	if (success == false) {
		// Errors at this point are unrecoverable	
		// so we send our error to the main process
		// and then go into a dead loop.
		printf("Error: %s\n", error);

	}

	printf("Heading into main loop\n");

	// Main loop
	for (;;) {
		_message_queue_process();
		_command_buffer_execute();

		// If the modem is off and we have been issued a 
		if (_start_command_issued && _modem_state == MODEM_STOPPED) {
			if (sim7080g_is_ready(_gateway) && sim7080g_config(_gateway)) {
				_modem_state = MODEM_STARTED;
				printf("modem started\n");
			}
			else if (_modem_power_toggled == false) {
				printf("togging power\n");
				sim7080g_toggle_power(_gateway);
				_modem_power_toggled = true;
			}
		} 
		else if (_start_command_issued) _start_command_issued = false;
		else if (_modem_state == MODEM_STARTED)
			if (sim7080g_is_ready(_gateway) == false)
				_modem_state = MODEM_STOPPED;

		// If we are in a powered on state and a CN is available
		// activate it
		if (_modem_state == MODEM_STARTED && _modem_cn_available()) {
			printf("trying to activate\n");
			if (sim7080g_cn_activate(_gateway, true)) {
				_modem_state = MODEM_CN_ACTIVE;
				printf("cn activated\n");
			}
		}
		// It the CN is active, connect to server
		else if (_modem_state == MODEM_CN_ACTIVE)
			if (sim7080g_cn_is_active(_gateway) == false)
				_modem_state = MODEM_STARTED;


		if (_modem_state == MODEM_CN_ACTIVE && sim7080g_ssl_enable(_gateway,false)) {
			sleep_ms(1000);
			if (sim7080g_tcp_open(_gateway, strlen(SERVER_IP), SERVER_IP, 8086))
				_modem_state = MODEM_SERVER_CONNECTED;
		}

		else if (_modem_state == MODEM_SERVER_CONNECTED) {
			char *message = "Hello!";
			uint16_t packet[10] = {[0] = htons(0), [1] = htons(strlen(message))};
			memcpy(&packet[2], message, strlen(message));
			sim7080g_tcp_send(_gateway, strlen(message) + (sizeof (uint16_t) * 2), (uint8_t *)packet);
			printf("sent\n");
		}

		printf("state: %u\n", _modem_state);

LOOP_CONTINUE:
		sleep_ms(1000);
	}
}

static bool _modem_cn_available(void) {
	if (_modem_state ==  MODEM_UNINITIALIZED || _modem_state == MODEM_STOPPED)
		return false;

	return sim7080g_cn_available(_gateway);
}

#define GW_Q_BUF_SIZE 400
#define GW_Q_BUF_MALLOC_FAILED -6
static int _message_queue_process(void) {
	uint32_t buffer = 0;
	uint16_t received = 0;
	uint16_t new_received;

	// Read until return is 0 or < 0 (indicating error)
	while (new_received = gw_queue_gw_recv((uint8_t *)&buffer, (sizeof buffer))) {

		printf("received: %u\n", new_received);

		if (new_received < 0) gw_core_panic(GW_CORE_FAILURE);

		received += new_received;
		// dispatch each uint32_t read to message buffer
		_dispatch_message_buffer(buffer);
	}
	
	return received;
}

static void _dispatch_message_buffer(uint32_t buffer) {
	static bool new_message = true;
	static uint32_t message_type;

	printf("nm: %s\n", new_message ? "true" : "false");
	printf("buffer: %u\n", buffer);

	if (new_message) {
		message_type = buffer;
		new_message = false;
	} else {
		uint rval = 0;
		switch(message_type) {
		case GATEWAY_COMMAND:
			rval = _build_command(buffer);
			// Unrecoverable error if rval < 0
			if (rval < 0) gw_core_panic(GW_CORE_FAILURE);
			if (rval == COMMAND_DISPATCHED) new_message = true;
			break;
		case PACKED_DATA:
			printf("data: %u\n", buffer);
			new_message = true;
			break;
		}
	}
}

static int _build_command(uint32_t buffer) {
	static bool new_command = true;
	static uint32_t command_type;

	if (new_command) {
		command_type = buffer;
		printf("ct: %u\n", command_type);
		new_command = false;
	}

	uint rval = COMMAND_UNRECOGNIZED;

	switch (command_type) {
	case GATEWAY_START:
    case GATEWAY_STOP:
		if (_command_buffer_push(&buffer, (sizeof buffer))) {
			printf("cb pushed\n");
			rval = COMMAND_DISPATCHED;
		}
		else rval = COMMAND_BUFFER_FULL;
		break;
	}

	// TODO: Need some better way to deal with full command buffer. How to reissue command or tell
	// main core that command failed?
	if (rval == COMMAND_DISPATCHED || rval == COMMAND_BUFFER_FULL) new_command = true;

	return rval;
}

static bool _command_buffer_push(void *command, uint size) {
	printf("cbr: %u | size: %u\n", cbuffer_remaining(_modem_buffer_command), size);
	if (size > cbuffer_remaining(_modem_buffer_command)) return false;	

	cbuffer_push(_modem_buffer_command, command, size);

	return true;
}

static void _command_buffer_execute(void) {
	static bool new_command = true;
	static uint32_t command_type;

	while (!cbuffer_empty(_modem_buffer_command)) {
		if (new_command) {
			cbuffer_pop(_modem_buffer_command, &command_type, (sizeof command_type));
			new_command = false;
		}

		switch (command_type) {
		case GATEWAY_START:
			_start_command_issued = true;
			new_command = true;
			break;
		case GATEWAY_STOP:
			printf("here stopping\n");
			if (sim7080g_power_down(_gateway)) _modem_state = MODEM_STOPPED;
			new_command = true;
			break;	
		}
	}
}

//void modem_test(void) {
//	printf("Starting modem... ");
//
//	Modem *modem = modem_start(
//			MODEM_APN,
//			UART_PORT,
//			UART_PIN_TX,
//			UART_PIN_RX,
//			MODEM_PIN_PWR
//	);
//
//	if (modem) printf("success!\n");
//	else {
//		printf("fail\n");
//		return;
//	}
//	
//	if (modem_cn_activate(modem, true)) printf("Network activated\n");
//
//	if (modem_ssl_enable(modem, false)) printf("SSL disabled\n");
//
//	if (modem_tcp_open(modem, strlen(SERVER_URL), SERVER_URL, SERVER_PORT)) {
//		printf("TCP connection opened\n");
//	}
//
//	uint8_t *msg = "PING";
//	if (modem_tcp_send(modem, strlen(msg), msg)) printf("Data sent!\n");
//
//	if (modem_tcp_recv_ready_within_us(modem, 1000 * 1000 * 10)) {
//		uint8_t dst[100] = {0};
//		size_t received = modem_tcp_recv(modem, 100, dst);
//		printf("received: %u\n", received);
//		printf("%.*s\n", received, dst);
//	}
//
//	if (modem_tcp_close(modem)) printf("TCP connection closed\n");
//
//	if (modem_cn_activate(modem, false)) printf("Network deactivated\n");
//
//	if (modem_power_down(modem)) printf("Modem powered down\n");
//}
