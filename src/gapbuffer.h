#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H


#include "terminal.h"

// Make sure to use array of lines ( instead of a single buffer later along the line)
typedef struct {
	char *buffer;
	int pos;
	int gap;
	int size;
	int left;
	int right;
} bufferConfig;

void updateBuffer(char ch, bufferConfig *B, editorConfig *E);

#endif
