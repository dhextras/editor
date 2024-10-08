#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>
#include <termios.h>

typedef struct {
	struct termios termios_default;
	bool escape_enabled;
} editorConfig;

void clearTerminal();
void failed(const char *func);
void disableTty(editorConfig *E);
void enableTty(editorConfig *E);

#endif
