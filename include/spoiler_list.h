#ifndef SPOILER_LIST_H
#define SPOILER_LIST_H

#include "spoiler.h"

struct spoiler_list {
    struct spoiler *spl;
    struct spoiler_list *next;
    struct spoiler_list *prev;
};

struct spoiler_list * spl_list_alloc(void);
void spl_list_append(struct spoiler_list *spl_list, struct spoiler *spl);
void spl_list_draw(struct spoiler_list *spl_list);
void spl_list_emit_toggle(struct spoiler_list *spl_list, int x, int y);
void spl_list_emit_scroll(struct spoiler_list *spl_list, int x, int y);
void spl_list_free(struct spoiler_list *spl_list);

#endif /* SPOILER_LIST_H */
