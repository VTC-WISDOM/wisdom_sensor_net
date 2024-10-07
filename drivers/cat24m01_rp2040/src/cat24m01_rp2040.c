#include "cat24m01_rp2040.h"

bool cat24_init(cat24_inst_t *_eeprom) {

//init the i2c line and check if it responds


}

bool cat24_is_busy(cat24_inst_t *_eeprom);
//does a selective read to check if the device ACKs

bool cat24_write_byte(
		cat24_inst_t *_eeprom, 
		uint16_t _page, 
		uint8_t _address, 
		uint8_t _data) {


	printf("enter write function\n");
	if(_page > 0x1FF) return 1;
	uint8_t _device_address = 0xA0;

	uint8_t _device_page = _page & 0x00FF;
	if(_eeprom->a2) _device_address |= 0x08;
	if(_eeprom->a1) _device_address |= 0x04;
	if(_page > 0x0F) _device_address |= 0x02;

	uint8_t _payload[3];
	_payload[0] = _page;
	_payload[1] = _address;
	_payload[2] = _data;

	printf("i2c_write_blocking(eeprom->i2c, %x, %x%x%x, 3, 0)\n", _device_address>>1, _payload[0], _payload[1], _payload[2]);
	int _status = i2c_write_blocking(_eeprom->i2c, _device_address>>1, (void *)&_payload, 3, 0);
	printf("status: %i\n", _status);
	if(_status != PICO_ERROR_GENERIC) return 0;
	else return 1;
}

bool cat24_write_data(
		cat24_inst_t *_eeprom, 
		uint16_t _page, 
		uint8_t _address,
		uint8_t _size,
		uint8_t *_data);
//write up to 256 bytes to a page starting at an address

bool cat24_read_immediate_byte(
		cat24_inst_t *_eeprom,
		uint8_t *_result);

//read a single byte from where the address pointer already is

bool cat24_read_selective_byte(
		cat24_inst_t *_eeprom,
		uint16_t _page,
		uint8_t _address,
		uint8_t *_result) {

	uint8_t _buf = 0x00;

	uint8_t _device_address = 0xA0;

	uint8_t _device_page = _page & 0x00FF;
	if(_eeprom->a2) _device_address |= 0x08;
	if(_eeprom->a1) _device_address |= 0x04;
	if(_page > 0x0F) _device_address |= 0x02;

	uint8_t _payload[2];
	_payload[0] = _page;
	_payload[1] = _address;


	printf("reading\n");
	printf("i2c_write_blocking(eeprom->i2c, %x, %x%x, 2, 1)\n", _device_address>>1, _payload[0], _payload[1]);
	
	int _status = i2c_write_blocking(_eeprom->i2c, _device_address>>1, (void *)&_payload, 2, 1);
	
	printf("wrote pointer status: %i\n", _status);

	//if(_status == PICO_ERROR_GENERIC) return 1;
	
	_status = i2c_read_blocking(_eeprom->i2c, _device_address>>1, &_buf, 1, 0);
	
	printf("read status: %i\n", _status);
	if(_status == PICO_ERROR_GENERIC) return 1;

	_result = &_buf;

}

//read a single byte from a selected page and address

bool cat24_read_immediate(
		cat24_inst_t *_eeprom,
		uint8_t _size,
		uint8_t *_result);
//read _size bytes (up to 256) from where the address pointer already is

bool cat24_read_selective(
		cat24_inst_t *_eeprom,
		uint16_t _page,
		uint8_t address,
		uint8_t _size,
		uint8_t *_result);
//read _size bytes from a selected page and address

