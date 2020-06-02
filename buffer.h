#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <windows.h>

#include "handle.h"
#include "data.h"

#define BUF_BAS_SIZE_X 60
#define BUF_BAS_SIZE_Y 34

struct buffer {
	CHAR_INFO *buf;
	int size_x;
	int size_y;
	
	/* reserved */
	int rsv_lines;
};

struct buffer * buf_alloc(const COORD wsize);
void buf_capture(struct buffer *buf);
void buf_draw(struct buffer *buf, int y_offset, int lines);
bool buf_compare(struct buffer *buf, struct buffer *buf_old);
void buf_commit(struct buffer *buf, CHAR_INFO *data, int size, int x, int y);
void buf_printf(int x, int y, WORD attrs, int size, const char *fmt, ...);
int buf_measure(struct buffer *buf);
void buf_copy(struct buffer *dst, struct buffer *src);
void buf_free(struct buffer *buf);

struct buffer *buf_bas_old;
struct buffer *buf_bas;
struct buffer *buf_rsv_old;
struct buffer *buf_rsv;

#endif /* BUFFER_H */
