#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "screen.h"
#include "terminal.h"
#include "gapbuffer.h"


void drawCursor(bufferConfig *B)
{
	dprintf(STDOUT_FILENO, "\x1b[%d;%dH", B->currentLine + 1, B->currentPos + 1);
}

void drawDebugger(bufferConfig *B, BufferLine *buff_line)
{
	write(STDOUT_FILENO, "\x1b[50;1H", 7);
	clearCurrLine();
	write(STDOUT_FILENO, "Total Line: ", 11);
	dprintf(STDOUT_FILENO, "%d ", B->numLines);
	write(STDOUT_FILENO, "| Current Line: ", 16);
	dprintf(STDOUT_FILENO, "%d ", B->currentLine);
	write(STDOUT_FILENO, "| Pos: ", 7);
	dprintf(STDOUT_FILENO, "%d ", B->currentPos);
	write(STDOUT_FILENO, "| Last Pos: ", 12);
	dprintf(STDOUT_FILENO, "%d ", B->lastPos);
	write(STDOUT_FILENO, "| Left: ", 8);
	dprintf(STDOUT_FILENO, "%d ", buff_line->left);
	write(STDOUT_FILENO, "| Right: ", 9);
	dprintf(STDOUT_FILENO, "%d ", buff_line->right);
	write(STDOUT_FILENO, "| Size: ", 8);
	dprintf(STDOUT_FILENO, "%d ", buff_line->size);
	drawCursor(B);
}

void drawLine(bufferConfig *B, BufferLine *buff_line)
{
	drawCursor(B);
	clearCurrLine();
	write(STDOUT_FILENO, buff_line->buffer, buff_line->left); // before the gap
	write(STDOUT_FILENO, &buff_line->buffer[buff_line->right + 1], buff_line->size - buff_line->right - 1); // after the gap
	drawCursor(B);
	drawDebugger(B, buff_line);
}
void drawScreen(bufferConfig *B)
{
	if (B->buffLines == NULL) {
		char *empty_screen_text = "Press Any key to start....";
		write(STDOUT_FILENO, empty_screen_text, strlen(empty_screen_text));

		return;
	}

	for (int i = 0; i < B->numLines; i++) {
		drawLine(B, B->buffLines[i]);
	}
}

