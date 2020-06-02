#include "data.h"

CHAR_INFO *
data_alloc(const char *data, WORD attrs) {
	size_t size = strlen(data);
	CHAR_INFO *ci = calloc(sizeof(CHAR_INFO), size);
	unsigned int i;

	for (i = 0; i < size; i++) {
		ci[i].Char.AsciiChar = data[i];
		ci[i].Attributes = attrs;
	}
	
	return ci;
}
