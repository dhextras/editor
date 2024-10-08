#include <stdlib.h>

#include "memory.h"

void free_line_buff(char *line_buff)
{
	// make sure your only freeing it if its acutally exists which idk how tf to do rn.
	free(line_buff);
}
