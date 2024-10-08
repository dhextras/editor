#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gapbuffer.h"
#include "memory.h"


void left(bufferConfig *B)
{
	while (B->pos < B->left) {
		B->left--;
		if (B->left < B->right) {
			B->buffer[B->right] = B->buffer[B->left];
			B->buffer[B->left] = '\0';
		}
		B->right--;
	}
}

void right(bufferConfig *B)
{
	while (B->pos > B->left) {
		B->right++;
		if (B->left < B->right) {
			B->buffer[B->left] = B->buffer[B->right];
			B->buffer[B->right] = '\0';
		}
		B->left++;
	}
}

void move_gap(bufferConfig *B)
{
	if (B->pos < B->left) {
		left(B);
	} else {
		right(B);
	}
}

void grow_buff(bufferConfig *B)
{
	char *newBuff = (char *)realloc(B->buffer, 2 * B->size);

	if (newBuff == NULL) {
		perror("Failed to reallocate memory");
		free_line_buff(B->buffer);
		exit(0);
	}

	B->buffer = newBuff;
	B->left = B->size;
	B->size += B->size;
	B->right = B->size - 1;
}

void insert(char ch, bufferConfig *B)
{
	if (B->left > B->right) {
		grow_buff(B);
	}

	if (B->pos != B->left) {
		move_gap(B);
	}

	B->buffer[B->left] = ch;
	B->left++;
	B->pos++;
}

void delete(bufferConfig *B)
{
	if (B->pos != B->left) {
		move_gap(B);
	}

	if (B->pos > 0) {
		B->left--;
		B->buffer[B->left] = '\0';
		B->pos--;
	}
}

void move_cl(bufferConfig *B)
{
	if (B->pos < B->size - B->right + B->left && B->pos > 0) {
		B->pos--;
	}
}

void move_cr(bufferConfig *B)
{
	if (B->pos < B->size - B->right + B->left - 1 && B->pos > -1) {
		B->pos++;
	}
}

void handle_escapes(char ch, bufferConfig *B, editorConfig *E) {
	switch (ch) {
		case 'D':
			move_cl(B);
			E->escape_enabled = false;
			break;
		case 'C':
			move_cr(B);
			E->escape_enabled = false;
			break;
		case '[':
			break;
		default:
			E->escape_enabled = false;
	}
}

void updateBuffer(char ch, bufferConfig *B, editorConfig *E)
{
	switch (E->escape_enabled) {
		case true:
			handle_escapes(ch, B, E);
			break;
		case false:
			switch (ch) {
				case 127:
					delete(B);
					break;
				case 13:
					// gotta create new line in the buffer
					break;
				case '\x1b':
					E->escape_enabled = true;
					break;
				case 'q':
					free_line_buff(B->buffer);
					disableTty(E);
					exit(0);
					break;
				default:
					insert(ch, B);
			}
	}
}

