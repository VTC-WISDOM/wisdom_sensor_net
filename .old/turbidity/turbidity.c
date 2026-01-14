#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include <string.h>
#include <stdio.h>

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#define OLED_WIDTH 128
#define OLED_HEIGHT 32

#define SENSOR_PIN 26
#define SENSOR_ADC 0

int i;
int turbidity_raw;
char str[20];
float volts, ntu;

int main()
{

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    sleep_ms(500);

    //oled inits
    ssd1306_t oled;
    oled.external_vcc = 0;
    ssd1306_init(&oled, OLED_WIDTH, OLED_HEIGHT, 0x3c, I2C_PORT);
    ssd1306_poweron(&oled);
    ssd1306_clear(&oled);
    ssd1306_draw_string(&oled, 0, 0, 1, "meow");
    ssd1306_show(&oled);

    sleep_ms(1000);

    //adc inits for turbidity sensor
    adc_init();
    adc_gpio_init(SENSOR_PIN);

    while(1) {
	    adc_select_input(SENSOR_ADC);
	    
	    for(i = 0, turbidity_raw = 0; i < 100; i++) {
	    turbidity_raw += adc_read();
	    }
	    turbidity_raw /= 100;

	    volts = ((float)turbidity_raw / 4095) * 5;

	    ntu = (-1120.4 * (volts * volts)) + (5742.3 * volts) - 4352.9;

	    sprintf(str, "%f", ntu);
	    ssd1306_clear(&oled);
	    ssd1306_draw_string(&oled, 0, 0, 1, str);
	    sprintf(str, "%f", volts);
	    ssd1306_draw_string(&oled, 0, 12, 1, str);
	    ssd1306_show(&oled);
	    sleep_ms(1000);
	    

    }

    return 0;
}
