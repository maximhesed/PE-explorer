#ifndef SPOILER_H
#define SPOILER_H

#include "colors.h"
#include "buffer.h"

enum SCROLL_STATE {
	SCROLL_HIDDEN,
	SCROLL_SHOWN
};

struct spoiler {
	char *caption;
	char **data;
	
	/* reserved */
	int x;
	int y;
	int items_q;
	int offset;
	bool state;
};

struct spoiler * spl_alloc(const char *cp, char *data);
void spl_draw(const struct spoiler *spl);
void spl_move(struct spoiler *spl, int y);
void spl_data_show(const struct spoiler *spl);
void spl_data_hide(const struct spoiler *spl);
void spl_data_toggle(struct spoiler *spl);
void spl_scroll_update(const struct spoiler *spl, int x, int y);
void spl_free(struct spoiler *spl);

#endif /* SPOILER_H */
