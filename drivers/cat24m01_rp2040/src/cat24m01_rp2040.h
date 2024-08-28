
typedef struct cat24m01_inst {
	bool a1; //
	bool a2; //a1 and a2 correspond to pins on the device
	i2c_inst_t *i2c;
	uint pin_scl;
	uint pin_sda;
} cat24_inst_t;

//addressing:
// slave address: 1010 followed by 2 bits corresponding to a1, a2, followed by a16 as part of the address, followed by R/W (1/0) bit
// for example to perform a read operation on the device with A1 and A2 set to 0, and to read from page 512 bit 1:
// 1010 0011 1111 0001
//
//
// all functions return a 0 for success and a 1 for failure
//
//



bool cat24_init(cat24_inst_t *_eeprom);
//init the i2c line and check if it responds

bool cat24_is_busy(cat24_inst_t *_eeprom);
//does a selective read to check if the device ACKs

bool cat24_write_byte(
		cat24_inst_t *_eeprom, 
		uint16_t _page, 
		uint8_t _address, 
		uint8_t _data);
//write one byte to a specified page and address

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
		uint8_t *_result);
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

