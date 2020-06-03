#include "buffer.h"

struct buffer *
buf_alloc(COORD wsize) {
	struct buffer *buf = malloc(sizeof(struct buffer));
	
	buf->buf = calloc(sizeof(CHAR_INFO), wsize.X * wsize.Y);
	buf->size_x = wsize.X;
	buf->size_y = wsize.Y;
	buf->rsv_lines = 0;
	
	return buf;	
}

void
buf_capture(struct buffer *buf) {
	COORD size = {buf->size_x, buf->size_y};
	SMALL_RECT rect = {0, 0, size.X - 1, size.Y - 1};
	
	ReadConsoleOutput(hout, buf->buf, size, (COORD) {0, 0}, &rect);
}

void
buf_draw(const struct buffer *buf, int y_offset, int lines) {
	COORD size = {BUF_BAS_SIZE_X, BUF_BAS_SIZE_Y - y_offset};
	SMALL_RECT rect = {0, 0, size.X - 1, size.Y - 1};
	unsigned int offset;
	
	if (lines > 0) {
		rect.Top = size.Y - lines;
		size.Y = lines;
	}
	
	if (y_offset > BUF_BAS_SIZE_Y) {
		y_offset -= BUF_BAS_SIZE_Y;
		
		size.Y = BUF_BAS_SIZE_Y;
		rect.Bottom = size.Y - 1;
	}
	
	offset = BUF_BAS_SIZE_X * y_offset;

	WriteConsoleOutput(hout, buf->buf + offset, size, (COORD) {0, 0}, &rect);
}

bool
buf_compare(const struct buffer *buf, const struct buffer *buf_old) {
	int i;
	
	for (i = 0; i < buf->size_x * buf->size_y; i++) {
		if (buf->buf[i].Char.AsciiChar != buf_old->buf[i].Char.AsciiChar 
				|| buf->buf[i].Attributes != buf_old->buf[i].Attributes)
			return false;
	}
	
	return true;
}

void
buf_commit(struct buffer *buf, CHAR_INFO *data, size_t size, int x, int y) {
	unsigned int i;
	int offset = (buf->size_x * y) + x;
	
	for (i = 0; i < size; i++) {
		if ((int) i + offset >= buf->size_x * buf->size_y) {
			buf_commit(buf_rsv, data + i, size - i, x, y - buf->size_y);
			
			return;
		}
		
		buf->buf[i + offset].Char.AsciiChar = data[i].Char.AsciiChar;
		buf->buf[i + offset].Attributes = data[i].Attributes;
	}
}

void
buf_printf(int x, int y, WORD attrs, size_t size, const char *fmt, ...) {
	CHAR_INFO *ci_data;
	char *data = calloc(sizeof(char), size + 1);
	va_list ap;
	
	va_start(ap, fmt);
	vsnprintf(data, size, fmt, ap);
	va_end(ap);
	
	ci_data = data_alloc(data, attrs);
	buf_commit(buf_bas, ci_data, size, x, y);
	
	free(ci_data);
	free(data);
}

int
buf_measure(const struct buffer *buf) {
	int i;
	int j;
	char c;
	
	for (i = buf->size_y - 1; i >= 0; i--) {
		for (j = 0; j < buf->size_x; j++) {
			c = buf->buf[i * buf->size_x + j].Char.AsciiChar;
			
			if (c != 0 && c != ' ')
				goto not_empty;
		}
	}
	
not_empty:
	return i + 1;
}

void
buf_copy(struct buffer *dst, const struct buffer *src) {
	int i;
	
	for (i = 0; i < src->size_x * src->size_y; i++) {		
		if (dst->buf[i].Char.AsciiChar != src->buf[i].Char.AsciiChar)
			dst->buf[i].Char.AsciiChar = src->buf[i].Char.AsciiChar;
			
		if (dst->buf[i].Attributes != src->buf[i].Attributes)
			dst->buf[i].Attributes = src->buf[i].Attributes;
	}	
}

void
buf_free(struct buffer *buf) {
	free(buf->buf);
	free(buf);
}
