#include "spoiler_list.h"

typedef struct spoiler_list node_t;

static node_t *
node_alloc(node_t *next, node_t *prev) {
	node_t *node = malloc(sizeof(node_t));
	
	node->spl = NULL;
	node->next = next;
	node->prev = prev;
	
	return node;
}

struct spoiler_list *
spl_list_alloc(void) {
	struct spoiler_list *spl_list = malloc(sizeof(struct spoiler_list));
	
	spl_list->spl = NULL;
	spl_list->next = node_alloc(NULL, spl_list);
	spl_list->prev = node_alloc(spl_list, NULL);
	
	return spl_list;
}

void 
spl_list_append(struct spoiler_list *spl_list, struct spoiler *spl) {
	node_t *node = spl_list->prev;
	
	while ((node = node->next)->spl);
	
	spl->x = 0;
	spl->y = node->prev->spl ? node->prev->spl->y + 2 : 0;
	
	node->spl = spl;
	node->next = node_alloc(NULL, node);
}

void 
spl_list_draw(struct spoiler_list *spl_list) {
	node_t *node = spl_list;
	
	do
		spl_draw(node->spl);
	while ((node = node->next)->spl);
}

void 
spl_list_emit_toggle(struct spoiler_list *spl_list, int x, int y) {
	node_t *node = spl_list;
	node_t *node_target = NULL;
	struct spoiler *target;
	
	do {
		struct spoiler *spl = node->spl;
		
		if ((x >= spl->offset && x <= spl->offset + 2) && y == spl->y)
			node_target = node;
	}
	while ((node = node->next)->spl);
	
	if (!node_target)
		return;

	target = node_target->spl;
	
	switch (target->state) {
	/* hide */
	case SCROLL_SHOWN:
		node = node_target;
		
		spl_data_toggle(target);
		
		while ((node = node->next)->spl)
			spl_move(node->spl, -target->items_q);
			
		break;

	/* show */
	case SCROLL_HIDDEN:
		while ((node = node->prev)->spl != target)
			spl_move(node->spl, target->items_q);
			
		spl_data_toggle(target);
		
		break;
	}
}

void 
spl_list_emit_scroll_update(struct spoiler_list *spl_list, int x, int y) {
	node_t *node = spl_list;
	
	do
		spl_scroll_update(node->spl, x, y);
	while ((node = node->next)->spl);	
}

void 
spl_list_free(struct spoiler_list *spl_list) {
	node_t *node = spl_list;
	
	free(spl_list->prev);
	
	while ((spl_list = spl_list->next)->spl) {
		spl_free(node->spl);
		free(node);
		
		node = spl_list;
	}
	
	free(spl_list->next);
}
