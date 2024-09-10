/*** includes ***/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>


/*** data ***/
struct termios termios_default;


/*** terminal ***/
void failed(const char *func) {
	perror(func);
	exit(1);
}

void disableRaw() {
	// TSCAFLUSH - waits until all pending output written to terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_default) == -1) failed("tcsetattr");
}

void enableRaw() {
	if (tcgetattr(STDIN_FILENO, &termios_default) == -1) failed("tcgetattr");
	atexit(disableRaw);

	struct termios termios_raw = termios_default;

	/* c_iflag - input flag => Disable CTRL - S,Q,C 
	 * c_lflag - local flag => Disable echoing & canonical mode & CTRL - Z
	 * c_oflag - output flag
	 * c_cflag - control flag
	 * Other ones i have no idea why,
	 * but some one on the internet said thats how you enable raw mode so here im..
	 */
	termios_raw.c_cflag |= (CS8);
	termios_raw.c_oflag &= ~(OPOST);
	termios_raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	termios_raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

	termios_raw.c_cc[VMIN] = 0; // Max byte to read set to 1
	termios_raw.c_cc[VTIME] = 1; // Max read wait time

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_raw) == -1) failed("tcsetattr");
}


/*** init ***/
int main() {
	enableRaw();

	while(1) {
		char ch = '\0';
		if (read(STDIN_FILENO, &ch, 1) == -1 && errno != EAGAIN) failed("read");

		if (iscntrl(ch)) {
			printf("%d\r\n", ch); // `\n` new line will flush the output immediately
		} else {
			printf("%d ('%c')\r\n", ch, ch);
		}

		if (ch == 'q') break;
	};

	return 0;
}
