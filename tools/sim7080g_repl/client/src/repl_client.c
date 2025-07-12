// C library headers
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h> // isprint
#include <errno.h>

#include <stdbool.h>

#include <fcntl.h> // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions

#include "rp2x_serial.h"

typedef unsigned uint;

bool send_command(int fd, const char *command, size_t command_len) {

		int rval = 0;
		while (command_len) {
			rval = write(fd, command, command_len);
			if (rval <= 0) break;

			command += rval;
			command_len -= rval;
		}

		return !command_len;
}

int main(int argc, char **argv) {

	printf("Sim7080G RP2X Serial Repl\n");

	int serial_port = rp2x_serial_open("/dev/ttyACM5", 10, 0, B115200);
	perror("serial");
	if (serial_port == -1) return 1;
	
	struct termios tty;
	struct termios term_recovery;

	// Read in existing settings, and handle any error
	if(tcgetattr(STDIN_FILENO, &tty) != 0)
		goto RECOVER;
	term_recovery = tty;

	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo

	tty.c_lflag &= ~ICANON; // Disable canonical mode
	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

	// Save tty settings, also checking for error
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tty) != 0)
		goto RECOVER;

	// Set both FDs to non-blocking
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
	flags = fcntl(serial_port, F_GETFL, 0);
    fcntl(serial_port, F_SETFL, flags | O_NONBLOCK);

	uint command_length_max = 100;
	char command[102] = {0};
	char output[100] = {0};
	int command_i = 0;
	int output_i = 0;

	int result;
	char c;
	size_t length = 0;

	for (;;) {

		int result = read(STDIN_FILENO, &c, 1);
		if (result == 1) {
			if (isprint(c)) {
				printf("%c", c);
				fflush(stdout);
				command[command_i++] = c;
			}
			else if (c == '\r') {
				printf("\r\n");
				command[command_i++] = c;
				send_command(serial_port, command, command_i);
				command_i = 0;
			}
			else if (c == 127 || c == 8) {
				printf("\b \b");
				fflush(stdout);
				if (command_i > 0) command_i--;
			}
		}

		while ((result = read(serial_port, &c, 1)) == 1) {
			if (c == '\n') {
				printf("\\n");
				printf("%s", "\r\n");
			}
			else if (c == '\r') printf("\\r");
			else
				printf("%c", c);
			fflush(stdout);
		}

		usleep(1000);
	}

RECOVER:

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_recovery);
	return 0;
};
