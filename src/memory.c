#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "gapbuffer.h"

#define INIT_BUFFER_SIZE 32

void create_buff_line(bufferConfig *B, char *text)
{
	int buff_size = INIT_BUFFER_SIZE;
	int text_len = strlen(text);

	while (buff_size <= text_len) {
		buff_size *= 2;
	}

	BufferLine *temp = malloc(sizeof(BufferLine));
	if (temp == NULL) {
		perror("Failed to allocate memory for new buffer line");
		return;
	}

	temp->size = buff_size;
	temp->left = text_len;
	temp->right = buff_size - 1;

	temp->buffer = malloc(buff_size);
	if (temp->buffer == NULL) {
		free(temp);
		perror("Failed to allocate memory for buffer");
		return;
	}

	strcpy(temp->buffer, text);

	BufferLine **new_buffLines = realloc(B->buffLines, (B->numLines + 1) * sizeof(BufferLine *));
	if (new_buffLines == NULL) {
		free(temp->buffer);
		free(temp);
		perror("Failed to reallocate memory for bufflines");
		return;
	}

	B->buffLines = new_buffLines;
	B->buffLines[B->numLines] = temp;
	B->numLines++;
	B->currentLine++;
	B->currentPos = temp->left;
}

void free_buff_config(bufferConfig *B)
{
	if (B == NULL || B->buffLines == NULL) {
		return;
	}

	for (int i = 0; i < B->numLines; i++) {
		if (B->buffLines[i] != NULL) {
			free(B->buffLines[i]->buffer);
			free(B->buffLines[i]);
		}
	}
	free(B->buffLines);
	B->buffLines = NULL;
	B->numLines = 0;
	B->currentLine = -1;
	B->currentPos = 0;
}
