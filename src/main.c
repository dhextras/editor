/*** includes ***/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "gapbuffer.h"
#include "screen.h"
#include "terminal.h"


/*** data ***/
editorConfig *E = &(editorConfig){ .escape_enabled = false };
bufferConfig *B = &(bufferConfig){
	.buffLines = NULL,
	.currentLine = -1,
	.currentPos = 0,
	.lastPos = 0,
	.numLines = 0
};

/*** init ***/
int main()
{
	enableTty(E);
	clearTerminal();
	drawScreen(B);

	while(1) {
		char ch = '\0';
		if (read(STDIN_FILENO, &ch, 1) == -1 && errno != EAGAIN) failed("read");

		update_buffer(ch, B, E);
	};

	disableTty(E);
	return 0;
}
