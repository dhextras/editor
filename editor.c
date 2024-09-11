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

void clearTerminal()
{
	// clear the console & set cursor to the start
	printf("\033[2J\033[1;1H");
	fflush(stdout);
}

void disableTty()
{
	// TSCAFLUSH - waits until all pending output written to terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_default) == -1) failed("tcsetattr");
	clearTerminal();
}

void enableTty()
{
	if (tcgetattr(STDIN_FILENO, &termios_default) == -1) failed("tcgetattr");
	atexit(disableTty);

	struct termios termios_tty = termios_default;

	/* c_iflag - input flag => Disable CTRL - S,Q,C 
	 * c_lflag - local flag => Disable echoing & canonical mode & CTRL - Z
	 * c_oflag - output flag
	 * c_cflag - control flag
	 * Other ones i have no idea why,
	 * but some one on the internet said thats how you enable raw mode so here im..
	 */
	termios_tty.c_cflag |= (CS8);
	termios_tty.c_oflag &= ~(OPOST);
	termios_tty.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	termios_tty.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

	// termios_tty.c_cc[VMIN] = 0; // Max byte to read set to 1
	// termios_tty.c_cc[VTIME] = 1; // Max read wait time

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_tty) == -1) failed("tcsetattr");
	clearTerminal();
}


/*** init ***/
int main()
{
	enableTty();

	while(1) {
		char ch = '\0';
		if (read(STDIN_FILENO, &ch, 1) == -1 && errno != EAGAIN) failed("read");

		if (iscntrl(ch)) {
			if (ch == 127) {
				printf("\b \b");
			} else if (ch == '\x1b') {
				printf("%c", ch);
			} else if (ch == 13) {
				printf("\r\n");
			} else if (ch == 9) {
				printf("\t");
			}
		} else {
			printf("%c", ch);
		}

		fflush(stdout); // `\n` new line will flush the output immediately
		if (ch == 'q') break;
	};

	return 0;
}
