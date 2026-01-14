/*
 * teros_11.c
 * 2023-06-09
 * amelia vlahogiannis
 * amelia@ag2v.com
 */

#include "teros_11.h"

teros_return teros_init(
		teros **teros, 
		uart_inst_t *serial, 
		teros_model model,
		int uart_tx_pin,
		int uart_rx_pin,
		int pwr_pin,
		teros_substrate substrate_type
		) {

	*teros = malloc(sizeof(teros));

	if(teros == NULL) return malloc_null;
	//save some struct elements to reference in other functions
	(*teros)->serial = serial;
	(*teros)->model = model;
	(*teros)->uart_tx_pin = uart_tx_pin;
	(*teros)->uart_rx_pin = uart_rx_pin;
	(*teros)->pwr_pin = pwr_pin;
	(*teros)->substrate_type = substrate_type;

	uart_init(serial, TEROS_BAUD_RATE);

	gpio_set_function(uart_rx_pin, GPIO_FUNC_UART);
	//gpio_set_function(uart_tx_pin, GPIO_FUNC_UART);
	gpio_init(pwr_pin);
	gpio_set_dir(pwr_pin, GPIO_OUT);
	
	return ok;
}

//grab and return all of the data
teros_return teros_get_data(teros *teros, teros_data *data) {
	teros_return status;
	unsigned char ch;
	unsigned char str[16];
	unsigned char vwc_str[8];
	unsigned char temp_str[8];
	unsigned char cond_str[16];
	unsigned char type;
	unsigned char check;
	unsigned char crc;
	double vwc_raw;
	double temp_raw;
	double cond_raw;
	int i, n;

	str[0] = '\0';	
	gpio_put(teros->pwr_pin, 1); //turn it on

	//wait for the uart to fill, time out if it doesnt after 150ms
	for(i = 0; !uart_is_readable(teros->serial) && i < 150; i += 10) sleep_ms(10);
	if(i > 140) return timed_out; //time out after 150ms

	for(i = 0; uart_is_readable_within_us(teros->serial, 30000);) { //not sure why it has to be within_us
									//but it wont work otherwise, or with 
									//less than 30000
		ch = uart_getc(teros->serial);
		if(isprint(ch) || ch == '\t' || ch == '\r') { //leave the tab/cr in for easier processing later
			str[i] = ch;
			i++;
		}
	}

	gpio_put(teros->pwr_pin, 0); //turn it off

	str[i] = '\0'; //terminate the string
	if(str[0] == '\0') return read_unsuccessful; //make sure it isnt zero length(this should not be possible)
	
	if(str[0] == '\t') { //just be absolutely certain its correct and starts with a tab
		for(i = 1; str[i] != ' '; i++) { //pull calibrated counts vwc out
			vwc_str[i - 1] = str[i];
		} vwc_raw = atof(&vwc_str); //convert string to float
		i++; //skip over a space

		for(n = 0; str[i] != ' ' && str[i] != '\r'; i++, n++) { //pull temperature
			temp_str[n] = str[i];
		} temp_raw = atof(&temp_str);
		i++; //skip over a space or carriage return

		if(teros->model == teros_12) { //pull conductivity if applicable(teros 12)
			for(n = 0; str[i] != ' ' && str[i] != '\r'; i++, n++) {
				cond_str[n] = str[i];
			} if(teros->model == teros_12) cond_raw = atof(&cond_str);
			i++;
		}

		type = str[i]; //sensor type
		i++;
		check = str[i]; //checksum
		i++;
		crc = str[i]; //crc6
	}	
	else return read_unsuccessful; //if its not a tab, bad read

	if(crc != _crc6_validate(&str)) return invalid_check_crc; //validate the checksum of the string
	if(check != _checksum_validate(&str)) return invalid_check_crc; //same with crc6
	
	//time to convert raw values to standard units
	if(teros->substrate_type == mineral) data->vwc = _raw_to_m3m3_mineral(vwc_raw);
	else if(teros->substrate_type == soilless) data->vwc = _raw_to_m3m3_soilless(vwc_raw);

	data->temperature = temp_raw;

	data->sensor_type = type;
	data->checksum = check;
	data->crc = crc;

	/*
	printf("\nWater Content(m^3/m^3): %.6f\n", data->vwc);
	printf("Temperature(C): %.1f\n", data->temperature);
	printf("Sensor type: %c\n", type);
	printf("Checksum:    %c\n", check);
	printf("CRC:         %c\n", crc);
	*/

	return ok;
}

//convert raw to m3/m3 for mineral soils
double _raw_to_m3m3_mineral(double raw) {
	double vwc;
	vwc = (0.0003879 * raw) - 0.6956;
	return vwc;
}

//convert raw to m3/m3 for soilless media
double _raw_to_m3m3_soilless(double raw) {
	double vwc;
	vwc = 0.0000000006771 * (raw * raw * raw) - 0.000005105 * (raw * raw) + 0.01302 * raw - 10.848;
	return vwc;
}



unsigned char _crc6_validate(const char *buffer) {
	//this function taken from the teros 11/12 integrator guide

	uint16_t byte;
	uint16_t i;
	uint16_t bytes;
	uint8_t bit;
	uint8_t crc = 0xfc;

	bytes = strlen(buffer);

	for(byte = 0; byte < bytes; byte++) {

		crc ^= buffer[byte];

		for(bit = 8; bit > 0; bit--) {

			if(crc & 0x80) crc = (crc << 1) ^ 0x9c;
			else crc = crc << 1;
		}

	if(buffer[byte] == '\r') bytes = byte + 3;
	}

	crc = (crc >> 2);
	return (crc + 48);
}

unsigned char _checksum_validate(const char *buffer) {
	//this function is also from the integrator guide
	uint16_t length;
	uint16_t i;
	uint16_t sum = 0;

	length = strlen(buffer);

	for(i = 0; i < length; i++) {
		sum += buffer[i];
		if(buffer[i] == '\r') break;
	}

	sum += buffer[++i];
	sum = sum % 64 + 32;

	return sum;
}
