#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#include "rfm69.h" 
#include "rudp.h"
#include "error_report.h"

#define ever ;; 

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_RST  20
#define PIN_IRQ_0  21
#define PIN_IRQ_1  21

#define PIN_I2C_SDA 2
#define PIN_I2C_SCK 3


void set_bi() {
    bi_decl(bi_program_name("RSSI Measuring Receiver"));
    bi_decl(bi_program_description("WISDOM sensor network."))
    bi_decl(bi_1pin_with_name(PIN_MISO, "MISO"));
    bi_decl(bi_1pin_with_name(PIN_CS, "CS"));
    bi_decl(bi_1pin_with_name(PIN_SCK, "SCK"));
    bi_decl(bi_1pin_with_name(PIN_MOSI, "MOSI"));
    bi_decl(bi_1pin_with_name(PIN_RST, "RST"));
    bi_decl(bi_1pin_with_name(PIN_I2C_SDA, "I2C SDA"));
    bi_decl(bi_1pin_with_name(PIN_I2C_SCK, "I2C SCK"));
    //bi_decl(bi_1pin_with_name(PIN_IRQ_0, "IRQ 0"));
    //bi_decl(bi_1pin_with_name(PIN_IRQ_1, "IRQ 1"));
}

int main() {
    // Set Picotool binary info
    set_bi();
    stdio_init_all(); // To be able to use printf

    spi_init(SPI_PORT, 1000*1000); // Defaults to master mode, which we want

    i2c_init(i2c1, 400000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

    ssd1306_t display;
    display.external_vcc = false;
    ssd1306_init(&display, 128, 32, 0x3C, i2c1);
    ssd1306_clear(&display);

    Rfm69 *rfm;
    uint rval = rfm69_init(
        &rfm,
        SPI_PORT,
        PIN_MISO,
        PIN_MOSI,
        PIN_CS,
        PIN_SCK,
        PIN_RST,
        PIN_IRQ_0,
        PIN_IRQ_1
    );

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    
    // 250kb/s baud rate
    rfm69_bitrate_set(rfm, RFM69_MODEM_BITRATE_57_6);
    // ~2 beta 
    rfm69_fdev_set(rfm, 70000);
    // 915MHz 
    rfm69_frequency_set(rfm, 915);
    // rfm69_modulation_shaping_set(rfm, RFM69_FSK_GAUSSIAN_0_3);
    // RXBW >= fdev + br/2
    rfm69_rxbw_set(rfm, RFM69_RXBW_MANTISSA_20, 2);
    rfm69_dcfree_set(rfm, RFM69_DCFREE_WHITENING);
    // Transmit starts with any data in the FIFO


    rfm69_node_address_set(rfm, 0x02); 

    // Check if rfm69_init was successful (== 0)
    // Set last error and halt process if not.
    if (rval != 0) {
        set_last_error(rval); // Can use return value from rfm69_init directly
        critical_error();
    }

    rfm69_power_level_set(rfm, -2);
    rx_report_t report;
    bool success;

    ssd1306_draw_string(&display, 0, 0, 1, "test");
    //ssd1306_bmp_show_image(&display, cat_bmp, 128);
    ssd1306_show(&display);

    for(ever) { 

        uint8_t address;
        uint size = 100000;
        uint8_t payload[size];

        printf("Waiting for message\n");
        printf("...\n");
	ssd1306_draw_string(&display, 0, 0, 1, "            ");
	ssd1306_draw_string(&display, 0, 0, 1, "Waiting");
	ssd1306_show(&display);

        success = rfm69_rudp_receive(
                rfm,
                &report,
                &address,
                payload,
                &size,
                12000,
                30000
        );

        printf("Report\n");
        printf("------\n");
        printf("      tx_address: %u\n", report.tx_address);
        printf("      rx_address: %u\n", report.rx_address);
        printf("  bytes_expected: %u\n", report.bytes_expected);
        printf("  bytes_received: %u\n", report.bytes_received);
        printf("packets_received: %u\n", report.packets_received);
        printf("       acks_sent: %u\n", report.acks_sent);
        printf("      racks_sent: %u\n", report.racks_sent);
        printf("   rack_requests: %u\n", report.rack_requests);


        switch(report.return_status) {
            case RUDP_OK:
                printf("   return_status: RUDP_OK\n");
                break;
            case RUDP_TIMEOUT:
                printf("   return_status: RUDP_TIMEOUT\n");
                break;
            case RUDP_BUFFER_OVERFLOW:
                printf("   return_status: RUDP_BUFFER_OVERFLOW\n");
                break;
        }
        printf("         message: %s\n", payload);
        printf("\n");

        if (success) {
		int16_t rssi = 0;
		char rssi_string[8];

		if(!rfm69_rssi_measurment_get(rfm, &rssi)) {

		sprintf(rssi_string, "%i", rssi);

		ssd1306_clear(&display);
		ssd1306_draw_string(&display, 0, 0, 1, "receiving");
		ssd1306_draw_string(&display, 0, 16, 1, rssi_string);
		ssd1306_show(&display);
		}

		else {
			ssd1306_clear(&display);
			ssd1306_draw_string(&display, 0, 16, 1, "error");
			ssd1306_show(&display);
		}

		uint num_blinks = 3;
		for(; num_blinks; num_blinks--) {
			gpio_put(PICO_DEFAULT_LED_PIN, 1);
			sleep_ms(100);
			gpio_put(PICO_DEFAULT_LED_PIN, 0);
			sleep_ms(50);
            }
        }
    }
    
    return 0;
}


