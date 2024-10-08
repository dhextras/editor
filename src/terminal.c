#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "terminal.h"
#include "memory.h"


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


void disableTty(editorConfig *E)
{
	// TSCAFLUSH - waits until all pending output written to terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E->termios_default) == -1) failed("tcsetattr");
	clearTerminal();
}


void enableTty(editorConfig *E)
{
	if (tcgetattr(STDIN_FILENO, &E->termios_default) == -1) failed("tcgetattr");

	struct termios termios_tty = E->termios_default;

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
