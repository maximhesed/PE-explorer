#ifndef SPOILER_H
#define SPOILER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>

#include "colors.h"
#include "buffer.h"
#include "data.h"

enum SCROLL_STATE {
	SCROLL_HIDDEN,
	SCROLL_SHOWN	
};

struct spoiler {
	char *caption;
	char **data; /* dynamic (!) */
	int x;
	int y;
	
	/* reserved */
	int items_q;
	int offset;
	bool state;
};

struct spoiler * spl_alloc(const char *cp, char *data);
void spl_draw(struct spoiler *spl);
void spl_move(struct spoiler *spl, int y);
void spl_data_show(struct spoiler *spl);
void spl_data_hide(struct spoiler *spl);
void spl_data_toggle(struct spoiler *spl);
void spl_scroll_update(struct spoiler *spl, int x, int y);
void spl_free(struct spoiler *spl);

#endif /* SPOILER_H */
