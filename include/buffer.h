#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdbool.h>

#include "handle.h"
#include "data.h"

#define BUF_BAS_SIZE_X 60
#define BUF_BAS_SIZE_Y 34
#define BUF_RSV_PAGES 64

struct buffer {
	CHAR_INFO *buf;
	int size_x;
	int size_y;
	
	/* reserved */
	int rsv_lines;
};

struct buffer * buf_alloc(COORD wsize);
void buf_capture(struct buffer *buf);
void buf_draw(const struct buffer *buf, int y_offset, int lines);
bool buf_compare(const struct buffer *buf, const struct buffer *buf_old);
void buf_commit(struct buffer *buf, CHAR_INFO *data, size_t size, int x, int y);
void buf_printf(int x, int y, WORD attrs, int size, const char *fmt, ...);
int buf_measure(const struct buffer *buf);
void buf_copy(struct buffer *dst, const struct buffer *src);
void buf_free(struct buffer *buf);

struct buffer *buf_bas;
struct buffer *buf_rsv;

#endif /* BUFFER_H */
