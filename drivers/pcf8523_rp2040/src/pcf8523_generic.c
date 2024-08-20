#include "pcf8523_generic.h"
#include "pcf8523_i2c_generic.h"

bool pcf8523_control_get_all(uint i2c_inst, uint8_t dst[3]) {
	bool success = false;

	// Set address to first control register.
	uint8_t reg = PCF8523_REG_CONTROL_1;
	if (!pcf8523_i2c_write(i2c_inst, &reg, 1))
		goto RETURN;	

	// Read all three control registers.
	if (!pcf8523_i2c_read(i2c_inst, dst, 3))
		goto RETURN;

	success = true;
RETURN:
	return success;
}


bool pcf8523_control_set_all(uint i2c_inst, uint8_t src[3]) {
	bool success = false;

	// Create buffer with register address first.
	uint8_t buf[4] = {PCF8523_REG_CONTROL_1, src[0], src[1], src[2]};

	if (!pcf8523_i2c_write(i2c_inst, buf, 4))
		goto RETURN;	

	success = true;
RETURN:
	return success;
}


bool pcf8523_control_get(uint i2c_inst, uint reg, uint8_t *dst) {
	bool success = false;

	uint8_t address = 0xFF;
	switch (reg) {
	case 1:
		address = PCF8523_REG_CONTROL_1;
		break;
	case 2:
		address = PCF8523_REG_CONTROL_2;
		break;
	case 3:
		address = PCF8523_REG_CONTROL_3;
		break;
	default:
		goto RETURN;
	}

	if (!pcf8523_i2c_write(i2c_inst, &address, 1))
		goto RETURN;	

	if (!pcf8523_i2c_read(i2c_inst, dst, 1))
		goto RETURN;

	success = true;
RETURN:
	return success;
}


bool pcf8523_control_set(uint i2c_inst, uint reg, uint8_t *src) {
	bool success = false;

	uint8_t address = 0xFF;
	switch (reg) {
	case 1:
		address = PCF8523_REG_CONTROL_1;
		break;
	case 2:
		address = PCF8523_REG_CONTROL_2;
		break;
	case 3:
		address = PCF8523_REG_CONTROL_3;
		break;
	default:
		goto RETURN;
	}

	// Create buffer with register address first.
	uint8_t buf[4] = {address, *src};

	if (!pcf8523_i2c_write(i2c_inst, buf, 2))
		goto RETURN;	

	success = true;
RETURN:
	return success;
}


// typedef enum _CAP_SEL_MODE_E {
// 	CAP_SEL_7_PF    = 0, // 7pF load capacitance
// 	CAP_SEL_12_5_PF = 1  // 12.5pF load capacitance
// } CAP_SEL_MODE_T;
bool pcf8523_cap_sel_get(uint i2c_inst, uint *cap_sel_mode) {
	bool success = false;

	if (!pcf8523_control_get(i2c_inst, 1, (uint8_t *)cap_sel_mode))	
		goto RETURN;

	*cap_sel_mode = (*cap_sel_mode & 0x80) >> 7;

	success = true;
RETURN:
	return success;
}
 

bool pcf8523_cap_sel_set(uint i2c_inst, CAP_SEL_MODE_T cap_sel_mode) {
	bool success = false;

	uint8_t buf;
	if (!pcf8523_control_get(i2c_inst, 1, &buf))	
		goto RETURN;

	// Clear CAP_SEL bit
	buf &= ~0x80;
	buf |= cap_sel_mode << 7;

	if (!pcf8523_control_set(i2c_inst, 1, &buf))	
		goto RETURN;

	success = true;
RETURN:
	return success;
}


bool pcf8523_time_circuit_is_running(uint i2c_inst, bool *is_running) {
	bool success = false;

	uint8_t buf;
	if (!pcf8523_control_get(i2c_inst, 1, &buf))	
		goto RETURN;

	*is_running = !(buf & 0x20);

	success = true;
RETURN:
	return success;
}


//bool pcf8523_time_circuit_start(uint i2c_inst) {
//	bool success = false;
//
//	uint8_t buf;
//	if (!pcf8523_control_get(i2c_inst, 1, &buf))	
//		goto RETURN;
//
//	// Set STOP bit
//	buf |= 0x20;
//
//	if (!pcf8523_control_set(i2c_inst, 1, &buf))
//		goto RETURN;
//
//	success = true;
//RETURN:
//	return success;
//}
//bool pcf8523_time_circuit_stop(uint i2c_inst) {
//	bool success = false;
//
//	uint8_t buf;
//	if (!pcf8523_control_get(i2c_inst, 1, &buf))	
//		goto RETURN;
//
//	// Clear STOP bit
//	buf &= ~0x20;
//
//	if (!pcf8523_control_set(i2c_inst, 1, &buf))
//		goto RETURN;
//
//	success = true;
//RETURN:
//	return success;
//}


bool pcf8523_software_reset_initiate(uint i2c_inst) {
	bool success = false;

	uint8_t reset_command = 0x58; // See datasheet section 8.3
	if (!pcf8523_control_set(i2c_inst, 1, &reset_command))
		goto RETURN;

	success = true;
RETURN:
	return success;
};


//	typedef enum _HOUR_MODE_E {
//	 	HOUR_MODE_24 = 0, // 0-23 hour mode
//	 	HOUR_MODE_12 = 1  // 1-12 hour mode with AM/PM flag
//	} HOUR_MODE_T;
bool pcf8523_hour_mode_get(uint i2c_inst, uint *hour_mode) {
	bool success = false;

	success = true;
RETURN:
	return success;
}
 

bool pcf8523_hour_mode_set(uint i2c_inst, HOUR_MODE_T hour_mode) {
	bool success = false;

	success = true;
RETURN:
	return success;
}
