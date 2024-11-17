#ifndef SCREEN_H
#define SCREEN_H


#include "gapbuffer.h"

void drawCursor(bufferConfig *B);
void drawLine(bufferConfig *B, BufferLine *buff_line);
void drawDebugger(bufferConfig *B, BufferLine *buff_line);
void drawScreen(bufferConfig *B);

#endif
