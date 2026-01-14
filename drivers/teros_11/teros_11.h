/*
 * teros_11.h
 *
 * pi pico library for the teros 11/12 soil moisture sensor
 * reads and decodes a 1200 baud serial signal from the sensor
 *
 * https://www.metergroup.com/en/meter-environment/products/teros-11-soil-moisture-sensor
 *
 * 2023-06-09
 * amelia vlahogiannis
 * amelia@ag2v.com
 */


#ifndef TEROS_11_H
#define TEROS_11_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/malloc.h"
#define TEROS_BAUD_RATE 1200

typedef enum _return {
	ok			=  0,
	invalid_check_crc	= -1,
	uart_not_enabled	= -2,
	timed_out		= -3,
	read_unsuccessful	= -4,
	malloc_null		= -5,
} teros_return;

typedef enum _model {
	teros_11,
	teros_12,
} teros_model;

typedef enum _substrate {
	mineral,
	soilless,
} teros_substrate;

typedef struct _teros {
	uart_inst_t *serial;
	teros_model model;
	int uart_tx_pin;
	int uart_rx_pin;
	int pwr_pin;
	teros_substrate substrate_type;
} teros;

//<TAB><calibratedCountsVWC> <temperature> <electricalConductivity><CR><sensorType><Checksum><CRC>
typedef struct _data {
	float vwc;
	float temperature;
	float conductivity;
	unsigned char sensor_type;
	unsigned char checksum;
	unsigned char crc;
} teros_data;

teros_return teros_init(
		teros **teros,
		uart_inst_t *serial,
		teros_model model,
		int uart_tx_pin,
		int uart_rx_pin,
		int pwr_pin,
		teros_substrate substrate_type);
teros_return teros_get_data(teros *teros, teros_data *data);

double _raw_to_m3m3_mineral(double raw);
double _raw_to_m3m3_soilless(double raw);

unsigned char _crc6_validate(const char *buffer);
unsigned char _checksum_validate(const char *buffer);

#endif 
