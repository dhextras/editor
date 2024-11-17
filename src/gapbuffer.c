#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gapbuffer.h"
#include "screen.h"
#include "memory.h"
#include "terminal.h"

// handle jk or up down movement, deleting end of the line

void left(bufferConfig *B, BufferLine *buff_line)
{
	while (B->currentPos < buff_line->left) {
		buff_line->left--;
		if (buff_line->left < buff_line->right) {
			buff_line->buffer[buff_line->right] = buff_line->buffer[buff_line->left];
			buff_line->buffer[buff_line->left] = '\0';
		}
		buff_line->right--;
	}
}

void right(bufferConfig *B, BufferLine *buff_line)
{
	while (B->currentPos > buff_line->left) {
		buff_line->right++;
		if (buff_line->left < buff_line->right) {
			buff_line->buffer[buff_line->left] = buff_line->buffer[buff_line->right];
			buff_line->buffer[buff_line->right] = '\0';
		}
		buff_line->left++;
	}
}

void move_gap(bufferConfig *B, BufferLine *buff_line)
{
	if (B->currentPos < buff_line->left) {
		left(B, buff_line);
	} else {
		right(B, buff_line);
	}
}

void grow_buff(bufferConfig *B, BufferLine *buff_line)
{
	char *newBuff = (char *)realloc(buff_line->buffer, 2 * buff_line->size);

	if (newBuff == NULL) {
		perror("Failed to reallocate memory for growing buffer");
		free_buff_config(B);
		exit(0);
	}

	buff_line->buffer = newBuff;
	buff_line->left = buff_line->size;
	buff_line->size += buff_line->size;
	buff_line->right = buff_line->size - 1;
}

void insert(char ch, bufferConfig *B, BufferLine *buff_line)
{
	if (buff_line->left > buff_line->right) {
		grow_buff(B, buff_line);
	}

	if (B->currentPos != buff_line->left) {
		move_gap(B, buff_line);
	}

	buff_line->buffer[buff_line->left] = ch;
	buff_line->left++;
	B->currentPos++;
	B->lastPos = B->currentPos;
}

void delete(bufferConfig *B, BufferLine *buff_line)
{
	if (B->currentPos != buff_line->left) {
		move_gap(B, buff_line);
	}

	if (B->currentPos > 0) {
		buff_line->left--;
		buff_line->buffer[buff_line->left] = '\0';
		B->currentPos--;
		B->lastPos = B->currentPos;
	}
}

void move_cl(bufferConfig *B, BufferLine *buff_line)
{
	if (B->currentPos < buff_line->size - buff_line->right + buff_line->left && B->currentPos > 0) {
		B->currentPos--;
		B->lastPos = B->currentPos;
	}
}

void move_cr(bufferConfig *B, BufferLine *buff_line)
{
	if (B->currentPos < buff_line->size - buff_line->right + buff_line->left - 1 && B->currentPos > -1) {
		B->currentPos++;
		B->lastPos = B->currentPos;
	}
}

void move_cu(bufferConfig *B)
{
	if (B->currentLine > 0) {
		BufferLine *prev_buff_line_p = B->buffLines[B->currentLine - 1];
		int prev_buff_line_length = prev_buff_line_p->size - prev_buff_line_p->right + prev_buff_line_p->left - 1;

		B->currentLine--;

		if (B->lastPos > prev_buff_line_length) {
			B->currentPos = prev_buff_line_length;
		} else {
			B->currentPos = B->lastPos;
		}
	}
}

void move_cd(bufferConfig *B)
{
	if (B->currentLine < B->numLines - 1) {
		BufferLine *next_buff_line_p = B->buffLines[B->currentLine + 1];
		int next_buff_line_length = next_buff_line_p->size - next_buff_line_p->right + next_buff_line_p->left - 1;

		B->currentLine++;

		if (B->lastPos > next_buff_line_length) {
			B->currentPos = next_buff_line_length;
		} else {
			B->currentPos = B->lastPos;
		}
	}
}

void handle_escapes(char ch, bufferConfig *B, BufferLine *buff_line, editorConfig *E)
{
	switch (ch) {
		case 'D':
			move_cl(B, buff_line);
			E->escape_enabled = false;
			break;
		case 'C':
			move_cr(B, buff_line);
			E->escape_enabled = false;
			break;
		case 'A':
			move_cu(B);
			E->escape_enabled = false;
			break;
		case 'B':
			move_cd(B);
			E->escape_enabled = false;
			break;
		case '[':
			break;
		default:
			E->escape_enabled = false;
	}

	// NOTE: Remove this later cause this causes debugger printing even when the input didn't did anything - and figure out a way to handle that maybe return value from the move_cx functions and draw depending on that
	drawDebugger(B, buff_line);
}

void update_buffer(char ch, bufferConfig *B, editorConfig *E)
{
	char *empty = "";
	if (B->buffLines == NULL) {
		create_buff_line(B, empty);
	}

	BufferLine *current_buff_line_p = B->buffLines[B->currentLine];

	switch (E->escape_enabled) {
		case true:
			handle_escapes(ch, B, current_buff_line_p, E);
			drawCursor(B);
			break;
		case false:
			switch (ch) {
				case 127:
					delete(B, current_buff_line_p);
					drawLine(B, current_buff_line_p);
					break;
				case 13:
					// This only handle when the cursor is at the end of the buffer
					// TODO: Make sure to handle when its in the middle of the buffer/ lines and split the lines and make new ones
					create_buff_line(B, empty);
					current_buff_line_p = B->buffLines[B->currentLine];
					drawLine(B, current_buff_line_p);
					// finish from heree....
					break;
				case '\x1b':
					E->escape_enabled = true;
					break;
				case 'q':
					free_buff_config(B);
					disableTty(E);
					exit(0);
					break;
				default:
					insert(ch, B, current_buff_line_p);
					drawLine(B, current_buff_line_p);
			}
	}
}

