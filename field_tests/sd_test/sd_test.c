#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "rtc.h"

#include "ff.h"
#include "f_util.h"

#include "hw_config.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 20 
#define PIN_CS   10
#define PIN_SCK  18
#define PIN_MOSI 19

absolute_time_t time_since_start;
uint32_t ms_since_start;
char time_string[16];



int main()
{
    stdio_init_all();
    time_init();
    
    sleep_ms(5000);

    sd_card_t *sd = sd_get_by_num(0);
    FIL fil;
    FRESULT fr;
    UINT bw;
    BYTE work[FF_MAX_SS];
    char label[32];
    const char* const filename = "sd_write_test.txt";

    printf("starting\n");

    printf("mounting sd card ...\n");
    fr = f_mount(&sd->fatfs, sd->pcName, 1);
    if(fr != FR_OK) {
	    printf("error mounting sd card!\n");
	    printf("%s\n", FRESULT_str(fr));
	    while(1);
    } else printf("successfully mounted\n");

    printf("opening file: %s ...\n", filename);
    fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
    if(fr != FR_OK && fr != FR_EXIST) {
	    printf("error opening file!\n");
	    printf("%s\n", FRESULT_str(fr));
	    while(1);
    } else printf("successfully opened file\n");

    printf("writing to file ...\n");
    time_since_start = get_absolute_time();
    ms_since_start = to_ms_since_boot(time_since_start);
    sprintf(time_string, "%d", ms_since_start);
    if(!f_printf(&fil, "%s, mow\n", time_string)) {
		    printf("error writing to file!\n");
		    printf("%s\n", FRESULT_str(fr));
    } else printf("successfully wrote to file\n");

    printf("closing file ...\n");
    fr = f_close(&fil);
    if(fr != FR_OK) {
	    printf("error closing file!\n");
	    printf("%s\n", FRESULT_str(fr));
    } else printf("successfully closed file\n");

    printf("unmounting\n");
    f_unmount(sd->pcName);

    printf("complete");

    while(1);
    return 0;
}
