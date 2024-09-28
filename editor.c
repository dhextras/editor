/*** includes ***/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

/*** defines ***/
#define INIT_BUFFER_SIZE 32

/*** data ***/
typedef struct {
	struct termios termios_default;
	bool escape_enabled;
} editorConfig;

typedef struct {
	char *buffer;
	int pos;
	int gap;
	int size;
	int left;
	int right;
} bufferConfig;


editorConfig E = { .escape_enabled = false };
bufferConfig B = {
	.pos = 0,
	.gap = INIT_BUFFER_SIZE,
	.size = INIT_BUFFER_SIZE,
	.left = 0,
	.right = INIT_BUFFER_SIZE - 1
};

/*** memory management ***/
void free_line_buff(char *line_buff)
{
	// make sure your only freeing it if its acutally exists
	free(line_buff);
}

/*** terminal ***/
void clearTerminal()
{
	write(STDOUT_FILENO, "\x1b[2J", 4); // clear the terminal
	write(STDOUT_FILENO, "\x1b[H", 3); // put cursor in the 1:1 pos
}

void failed(const char *func)
{
	perror(func);
	free_line_buff(B.buffer);
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
		if (B.left < B.right) {
			B.buffer[B.right] = B.buffer[B.left];
			B.buffer[B.left] = '\0';
		}
		B.right--;
	}
}

void right()
{
	while (B.pos > B.left) {
		B.right++;
		if (B.left < B.right) {
			B.buffer[B.left] = B.buffer[B.right];
			B.buffer[B.right] = '\0';
		}
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

void grow_buff()
{
	char *newBuff = (char *)realloc(B.buffer, 2 * B.size);

	if (newBuff == NULL) {
		perror("Failed to reallocate memory");
		free_line_buff(B.buffer);
		exit(0);
	}

	B.buffer = newBuff;
	B.left = B.size + 1;
	B.right = B.size + B.gap;
	B.size += B.size;
}

void insert(char ch)
{
	if (B.left > B.right) {
		grow_buff();
	}

	if (B.pos != B.left) {
		move_gap();
	}

	B.buffer[B.left] = ch;
	B.left++;
	B.pos++;
}

void delete()
{
	if (B.pos != B.left) {
		move_gap();
	}

	if (B.pos > 0) {
		B.left--;
		B.buffer[B.left] = '\0';
		B.pos--;
	}
}

void move_cl()
{
	if (B.pos < B.size - B.right + B.left && B.pos > 0) {
		B.pos--;
	}
}

void move_cr()
{
	if (B.pos < B.size - B.right + B.left - 1 && B.pos > -1) {
		B.pos++;
	}
}

void handle_escapes(char ch) {
	switch (ch) {
		case 'D':
			move_cl();
			E.escape_enabled = false;
			break;
		case 'C':
			move_cr();
			E.escape_enabled = false;
			break;
		case '[':
			break;
		default:
			E.escape_enabled = false;
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
				case 13:
					// gotta create new line in the buffer
					break;
				case '\x1b':
					E.escape_enabled = true;
					break;
				case 'q':
					free_line_buff(B.buffer);
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
	dprintf(STDOUT_FILENO, "%d ", B.pos);
	write(STDOUT_FILENO, "Left: ", 6);
	dprintf(STDOUT_FILENO, "%d ", B.left);
	write(STDOUT_FILENO, "Right: ", 7);
	dprintf(STDOUT_FILENO, "%d ", B.right);
	write(STDOUT_FILENO, "Size: ", 6);
	dprintf(STDOUT_FILENO, "%d ", B.size);
	drawCursor();
}

void drawScreen()
{
	clearTerminal();
	write(STDOUT_FILENO, B.buffer, B.left); // before the gap
	write(STDOUT_FILENO, &B.buffer[B.right + 1], B.size - B.right - 1); // after the gap
	drawCursor();
	drawDebugger();
}


/*** init ***/
int main()
{
	B.buffer = (char *)malloc(INIT_BUFFER_SIZE);
	if (B.buffer == NULL) {
		perror("Failed to allocate memory");
		return 0;
	}

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
