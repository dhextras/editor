/*** includes ***/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "gapbuffer.h"
#include "screen.h"
#include "terminal.h"

/*** defines ***/
#define INIT_BUFFER_SIZE 32

/*** data ***/
editorConfig *E = &(editorConfig){ .escape_enabled = false };
bufferConfig *B = &(bufferConfig){
	.pos = 0,
	.gap = INIT_BUFFER_SIZE,
	.size = INIT_BUFFER_SIZE,
	.left = 0,
	.right = INIT_BUFFER_SIZE - 1
};


/*** init ***/
int main()
{
	B->buffer = (char *)malloc(INIT_BUFFER_SIZE);
	if (B->buffer == NULL) {
		perror("Failed to allocate memory");
		return 0;
	}

	enableTty(E);
	drawScreen(B);

	while(1) {
		char ch = '\0';
		if (read(STDIN_FILENO, &ch, 1) == -1 && errno != EAGAIN) failed("read");

		updateBuffer(ch, B, E);
		drawScreen(B);
	};

	disableTty(E);
	return 0;
}
