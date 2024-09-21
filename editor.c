/*** includes ***/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>


/*** data ***/
struct editorConfig {
	struct termios termios_default;
};

struct bufferConfig {
	char buffer[1024];
	int pos;
};

struct editorConfig E;
struct bufferConfig B = { .buffer = "Just checkign things out\r\nHello\tl...", .pos = 0 };

/*** terminal ***/
void clearTerminal()
{
	write(STDOUT_FILENO, "\x1b[2J", 4); // clear the terminal
	write(STDOUT_FILENO, "\x1b[H", 3); // put cursor in the 1:1 pos
}


void failed(const char *func)
{
	perror(func);
	exit(1);
}


void disableTty()
{
	// TSCAFLUSH - waits until all pending output written to terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.termios_default) == -1) failed("tcsetattr");
	clearTerminal();
}


void enableTty()
{
	if (tcgetattr(STDIN_FILENO, &E.termios_default) == -1) failed("tcgetattr");
	atexit(disableTty);

	struct termios termios_tty = E.termios_default;

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

/*** input ***/
void updateBuffer(char ch)
{
	switch (ch) {
		case 127:
			// fix it 
			B.buffer[B.pos] = ch;
			break;
		case 'q':
			exit(0);
			break;
		default:
			B.buffer[B.pos] = ch;
	}
}

/*** output ***/
void drawScreen()
{
	clearTerminal();
	write(STDOUT_FILENO, B.buffer, sizeof(B.buffer));
}


/*** init ***/
int main()
{
	enableTty();
	drawScreen();

	while(1) {
		char ch = '\0';
		if (read(STDIN_FILENO, &ch, 1) == -1 && errno != EAGAIN) failed("read");

		B.pos++;
		updateBuffer(ch);
		drawScreen();
	};

	return 0;
}
