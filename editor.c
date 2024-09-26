/*** includes ***/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>


/*** data ***/
typedef struct {
	struct termios termios_default;
	bool escape_enabled;
} editorConfig;

typedef struct {
	char buffer[64];
	int pos;
	int gap;
	int left;
	int right;
} bufferConfig;


editorConfig E = { .escape_enabled = false };
bufferConfig B = {
	.buffer = "________________________________________________________________",
	.pos = 0,
	.gap = 64,
	.left = 0,
	.right = 63

};

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
void left()
{
	while (B.pos < B.left) {
		B.left--;
		B.buffer[B.right] = B.buffer[B.left];
		B.buffer[B.left] = '_';
		B.right--;
	}
}

void right()
{
	while (B.pos > B.left) {
		B.right++;
		B.buffer[B.left] = B.buffer[B.right];
		B.buffer[B.right] = '_';
		B.left++;
	}
}

void move_gap()
{
	if (B.pos < B.left) {
		left();
	} else {
		right();
	}
}

void insert(char ch)
{
	if (B.pos != B.left) {
		move_gap();
	}

	if (B.left < B.right) {
		// else grow
		B.buffer[B.left] = ch;
		B.left++;
		B.pos++;
	}

	
}

void delete()
{
	if (B.pos > 0) {
		B.left--;
		B.buffer[B.left] = '_';
		B.pos--;
	}
}

void handle_escapes(char ch) {
	if (B.pos < 64 || B.pos > -1) {
		switch (ch) {
			case 'D':
				B.pos--;
				E.escape_enabled = false;
				break;
			case 'C':
				B.pos++;
				E.escape_enabled = false;
				break;
		}
	}
}

void updateBuffer(char ch)
{
	switch (E.escape_enabled) {
		case true:
			handle_escapes(ch);
			break;
		case false:
			switch (ch) {
				case 127:
					delete();
					break;
				case '\x1b':
					E.escape_enabled = true;
					break;
				case 'q':
					exit(0);
					break;
				default:
					insert(ch);
			}
	}
}


/*** output ***/
void drawCursor()
{
	dprintf(STDOUT_FILENO, "\x1b[1;%dH", B.pos + 1);
}

void drawDebugger()
{
	write(STDOUT_FILENO, "\x1b[32;1H", 7);
	write(STDOUT_FILENO, "Pos: ", 5);
	dprintf(STDOUT_FILENO, "%d\t", B.pos);
	write(STDOUT_FILENO, "Left: ", 6);
	dprintf(STDOUT_FILENO, "%d\t", B.left);
	write(STDOUT_FILENO, "Right: ", 7);
	dprintf(STDOUT_FILENO, "%d\t", B.right);
	write(STDOUT_FILENO, B.buffer, sizeof(B.buffer));
	drawCursor();
}

void drawScreen()
{
	clearTerminal();
	write(STDOUT_FILENO, B.buffer, B.left); // before the gap
	write(STDOUT_FILENO, &B.buffer[B.right + 1], 63 - B.right); // after the gap
	drawCursor();
	drawDebugger();
}


/*** init ***/
int main()
{
	enableTty();
	drawScreen();

	while(1) {
		char ch = '\0';
		if (read(STDIN_FILENO, &ch, 1) == -1 && errno != EAGAIN) failed("read");

		updateBuffer(ch);
		drawScreen();
	};

	return 0;
}
