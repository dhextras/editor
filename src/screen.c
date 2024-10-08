#include <stdio.h>
#include <unistd.h>

#include "screen.h"
#include "terminal.h"
#include "gapbuffer.h"


void drawCursor(bufferConfig *B)
{
	dprintf(STDOUT_FILENO, "\x1b[1;%dH", B->pos + 1);
}

void drawDebugger(bufferConfig *B)
{
	write(STDOUT_FILENO, "\x1b[32;1H", 7);
	write(STDOUT_FILENO, "Pos: ", 5);
	dprintf(STDOUT_FILENO, "%d ", B->pos);
	write(STDOUT_FILENO, "Left: ", 6);
	dprintf(STDOUT_FILENO, "%d ", B->left);
	write(STDOUT_FILENO, "Right: ", 7);
	dprintf(STDOUT_FILENO, "%d ", B->right);
	write(STDOUT_FILENO, "Size: ", 6);
	dprintf(STDOUT_FILENO, "%d ", B->size);
	drawCursor(B);
}

void drawScreen(bufferConfig *B)
{
	clearTerminal();
	write(STDOUT_FILENO, B->buffer, B->left); // before the gap
	write(STDOUT_FILENO, &B->buffer[B->right + 1], B->size - B->right - 1); // after the gap
	drawCursor(B);
	drawDebugger(B);
}

