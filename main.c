#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>

#include "handle.h"
#include "colors.h"
#include "buffer.h"
#include "spoiler.h"
#include "spoiler_list.h"
#include "log.h"
#include "pe.h"

/* [v] TODO: Solve a "lowest line" problem. */
/* [v] TODO: Add a vertical scroll ability. */
/* [v] TODO: Handle the spoiler scroll when the buffer is scrolled. */
/* [v] TODO: Don't handle the spoiler scroll, if it isn't visible. */
/* [v] TODO: Write a simple log system. */
/* [v] TODO: Expand the reserved buffer to (wsize.Y * 64). */
/* [v] TODO: Pass a data in this format: "data: value". */
/* [ ] TODO: Spoilers in the spoiler. */
/* [ ] TODO: Handle ": value" as the spoiler scroll. */
/* [ ] TODO: Uncover bitmasks, if possibly. */
/* [ ] TODO: Write a context menu. */
/* [ ] TODO: Add a horizontal scroll ability. */
/* [ ] TODO: Do free before the box closing. */
/* [ ] TODO: Spoilers drag and drop. */

int main(int argc, char *argv[]) {
	const COORD wsize = {BUF_BAS_SIZE_X, BUF_BAS_SIZE_Y};
	const SMALL_RECT rect = {0, 0, wsize.X - 1, wsize.Y - 1};
	const HWND winh = GetConsoleWindow();
	const LONG wlong = GetWindowLong(winh, GWL_STYLE);
	
	extern FILE *flog;
	extern PIMAGE_FILE_HEADER hfile;
	extern PIMAGE_OPTIONAL_HEADER hopt;
	extern PIMAGE_SECTION_HEADER hsec;
	extern HANDLE hout;
	extern HANDLE hin;
	extern struct buffer *buf_bas_old;
	extern struct buffer *buf_bas;
	extern struct buffer *buf_rsv_old;
	extern struct buffer *buf_rsv;
	
	FILE *file;
	DWORD file_len;
	char *bytes = NULL;
	INPUT_RECORD ev_buf;
	struct spoiler_list *spl_list;
	bool quit = false;
	int x;
	int y;
	int wheel_offset = 0;
	int wheel_state = 0;
	bool wheel = false;
	int i;
	
	if (argc != 2)
		return -1;

	file = CreateFileA(
		argv[1],
		GENERIC_READ, 
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (file == INVALID_HANDLE_VALUE)
		assert("CreateFileA");
	
	file_len = GetFileSize(file, NULL);
	
	bytes = calloc(file_len, sizeof(char));
	if (!ReadFile(file, bytes, file_len, &(DWORD) {0}, NULL))
		assert("ReadFile");
	
	flog = fopen("log.txt", "w");
	hfile = hfile_get(bytes);
	hopt = hopt_get(bytes);
	hsec = hsec_get(bytes);
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	hin = GetStdHandle(STD_INPUT_HANDLE);
	buf_bas_old = buf_alloc(wsize);
	buf_bas = buf_alloc(wsize);
	buf_rsv_old = buf_alloc((COORD) {wsize.X, wsize.Y * 64});
	buf_rsv = buf_alloc((COORD) {wsize.X, wsize.Y * 64});
	
	SetConsoleCursorInfo(hout, &(CONSOLE_CURSOR_INFO) {1, false});
	SetConsoleWindowInfo(hout, TRUE, &rect);
	SetConsoleScreenBufferSize(hout, wsize);
	SetWindowPos(winh, HWND_NOTOPMOST, 200, 100, 0, 0, SWP_NOSIZE);
	SetWindowLong(winh, GWL_STYLE, wlong & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX
		& ~ WS_MINIMIZEBOX);
	
	/* --- */
	
	buf_capture(buf_bas_old);
	buf_copy(buf_bas, buf_bas_old);
	
	spl_list = spl_list_alloc();

	spl_list_append(
		spl_list, 
		spl_alloc("IMAGE_FILE_HEADER", hfile_info_get()));
	spl_list_append(
		spl_list, 
		spl_alloc("IMAGE_OPTIONAL_HEADER", hopt_info_get()));
		
	/* Append all found section headers. */
	for (i = 0; i < hfile->NumberOfSections; i++) {
		IMAGE_SECTION_HEADER *sec = hsec + i;
		char cp[256];
		
		sprintf(cp, "IMAGE_SECTION_HEADER (%s)", sec->Name);
		
		spl_list_append(
			spl_list,
			spl_alloc(cp, hsec_info_get(sec)));
	}
	
	spl_list_draw(spl_list);
	
	while (!quit) {
		bool ev_skip = false;
		bool update = false;
		
		/* TODO: Re-write this simpler. */
		if (!buf_compare(buf_bas, buf_bas_old)) {
			buf_copy(buf_bas_old, buf_bas);
			
			update = true;
		}
		
		if (!buf_compare(buf_rsv, buf_rsv_old)) {
			buf_copy(buf_rsv_old, buf_rsv);
			
			buf_rsv->rsv_lines = buf_measure(buf_rsv);
			
			if (wheel_offset > buf_rsv->rsv_lines)
				wheel_offset = buf_rsv->rsv_lines;
				
			update = true;
		}
		
		if (wheel) {
			update = true;
			wheel = false;
		}
		
		if (update) {
			if (wheel_offset > 0) {
				if (wheel_offset > buf_bas->size_y)
					buf_draw(buf_rsv, wheel_offset, 0);
				else {
					buf_draw(buf_rsv, 0, wheel_offset);
					buf_draw(buf_bas, wheel_offset, 0);
				}	
			}
			else
				buf_draw(buf_bas, 0, 0);
			
			update = false;	
		}

		ReadConsoleInput(hin, &ev_buf, 1, &(DWORD) {0});
		
		switch (ev_buf.EventType) {
		case MOUSE_EVENT:
			x = ev_buf.Event.MouseEvent.dwMousePosition.X;
			y = ev_buf.Event.MouseEvent.dwMousePosition.Y + wheel_offset;
			
			switch (ev_buf.Event.MouseEvent.dwEventFlags) {	
			case MOUSE_MOVED:
				spl_list_emit_scroll_update(spl_list, x, y);
				
				ev_skip = true;
				
				break;
			
			case MOUSE_WHEELED:
				wheel_state = (int) ev_buf.Event.MouseEvent.dwButtonState;
				if (wheel_state > 0) {
					if (wheel_offset > 0) {
						wheel_offset -= 2;
						
						if (wheel_offset < 0)
							wheel_offset = 0;
					
						wheel = true;
					}
				}
				else {
					if (wheel_offset < buf_rsv->rsv_lines) {
						wheel_offset += 2;
						
						if (wheel_offset > buf_rsv->rsv_lines)
							wheel_offset = buf_rsv->rsv_lines;
					
						wheel = true;
					}
				}
				
				break;
			}
			
			if (ev_skip)
				break;
			
			switch (ev_buf.Event.MouseEvent.dwButtonState) {
			case FROM_LEFT_1ST_BUTTON_PRESSED:
				spl_list_emit_toggle(spl_list, x, y);
				
				break;
			}

			break;
			
		case KEY_EVENT:
			switch (ev_buf.Event.KeyEvent.wVirtualKeyCode) {
			case VK_F1:
				quit = true;
				
				break;
			}
			
			break;
		}
	}
	
	CloseHandle(file);
	fclose(flog);
	
	free(bytes);
	buf_free(buf_bas_old);
	buf_free(buf_bas);
	buf_free(buf_rsv_old);
	buf_free(buf_rsv);
	spl_list_free(spl_list);
	
	return 0;	
}
