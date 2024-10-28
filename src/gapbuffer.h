#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H


#include "terminal.h"

// Make sure to use array of lines ( instead of a single buffer later along the line)
typedef struct {
	char *buffer;
	int size;
	int left;
	int right;
} BufferLine;

typedef struct {
	BufferLine **buffLines;
	int currentLine;
	int currentPos;
	int numLines;
} bufferConfig;

void update_buffer(char ch, bufferConfig *B, editorConfig *E);

#endif
