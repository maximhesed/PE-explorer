#include <stdio.h>
#include <conio.h>
#include <windows.h>

#include "buffer.h"
#include "pe.h"
#include "spoiler.h"
#include "spoiler_list.h"

#define FILE_SIZE_MAX 256

static void DragInfo(void);
static void DragRead(char fname[FILE_SIZE_MAX]);

static void
DragInfo(void) {
	const char *str = "Drop a PE file here";
	
	COORD str_pos = {
		BUF_BAS_SIZE_X / 2 - strlen(str) / 2, 
		BUF_BAS_SIZE_Y / 2 - 1
	};
	COORD size = {BUF_BAS_SIZE_X, BUF_BAS_SIZE_Y};
	SMALL_RECT rect = {0, 0, size.X - 1, size.Y - 1};
	CHAR_INFO *data = malloc(sizeof(CHAR_INFO) * size.X * size.Y);
	int i;
	
	for (i = 0; i < size.X * size.Y; i++) {
		data[i].Char.AsciiChar = 177;
		data[i].Attributes = FOREGROUND_WHITE | 0;
	}
	
	WriteConsoleOutput(hout, data, size, (COORD) {0, 0}, &rect);
	
	SetConsoleTextAttribute(hout, 0 | BACKGROUND_WHITE);
	SetConsoleCursorPosition(hout, str_pos);
	printf(str);
	
	free(data);
}

static void
DragRead(char fname[FILE_SIZE_MAX]) {
	int dfnp = 0 - 1;
	char c;
	
	do {
		c = getch();
		
		if (c != '\"')
			fname[dfnp += 1] = c;
	}
	while (kbhit());
	
	fname[dfnp + 1] = '\0';
}

int main(int argc, char *argv[]) {
	const int WHEEL_OFFSET = 2;

	extern FILE *flog;
	extern HANDLE hout;
	extern HANDLE hin;
	extern struct buffer *buf_bas;
	extern struct buffer *buf_rsv;
	
	COORD wsize = {BUF_BAS_SIZE_X, BUF_BAS_SIZE_Y};
	SMALL_RECT rect = {0, 0, wsize.X - 1, wsize.Y - 1};
	HWND winh = GetConsoleWindow();
	LONG wlong = GetWindowLong(winh, GWL_STYLE);
	LONG flags = ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX;
	FILE *file;
 	DWORD file_len;
 	char *bytes = NULL;
	IMAGE_FILE_HEADER *hfile;
	IMAGE_OPTIONAL_HEADER *hopt;
	IMAGE_SECTION_HEADER *hsec;
	struct buffer *buf_bas_old;
	struct buffer *buf_rsv_old;
	INPUT_RECORD ev_buf;
	struct spoiler *spl_hfile;
	struct spoiler *spl_hopt;
	struct spoiler_list *spl_list;
	bool quit = false;
	bool wheel = false;
	int wheel_pos = 0;
	int i;
	char fname[FILE_SIZE_MAX];
	
	flog = fopen("log.txt", "w");
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	hin = GetStdHandle(STD_INPUT_HANDLE);
	buf_bas_old = buf_alloc(wsize);
	buf_bas = buf_alloc(wsize);
	buf_rsv_old = buf_alloc((COORD) {wsize.X, wsize.Y * BUF_RSV_PAGES});
	buf_rsv = buf_alloc((COORD) {wsize.X, wsize.Y * BUF_RSV_PAGES});
	
	SetConsoleCursorInfo(hout, &(CONSOLE_CURSOR_INFO) {1, false});
	SetConsoleWindowInfo(hout, TRUE, &rect);
	SetConsoleScreenBufferSize(hout, wsize);
	SetWindowPos(winh, HWND_NOTOPMOST, 200, 100, 0, 0, SWP_NOSIZE);
	SetWindowLong(winh, GWL_STYLE, wlong & flags);
	
	buf_capture(buf_bas_old);
	buf_copy(buf_bas, buf_bas_old);
	
	if (argc != 2) {
		bool valid = false;
		
		DragInfo();
		
		do {
			DragRead(fname);
			
			if (!strstr(fname, ".exe") && !strstr(fname, ".dll")) {
				MessageBox(winh, 
					"Select a file with an \".exe\" or \".dll\" extension.", 
					"Main", MB_OK | MB_ICONINFORMATION);
			
				continue;
			}

			valid = true;
		} 
		while (!valid);
	} else
		strncpy(fname, argv[1], FILE_SIZE_MAX);

	file = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		assert(winh, "CreateFileA");
	
	file_len = GetFileSize(file, NULL);
	
	bytes = calloc(sizeof(char), file_len);
	if (!ReadFile(file, bytes, file_len, &(DWORD) {0}, NULL))
		assert(winh, "ReadFile");

 	hfile = hfile_get(bytes);
	hopt = hopt_get(bytes);
	hsec = hsec_get(bytes);
	
	spl_hfile = spl_alloc("IMAGE_FILE_HEADER", hfile_info_get(hfile));
	spl_hopt = spl_alloc("IMAGE_OPTIONAL_HEADER", hopt_info_get(hopt));
	
	spl_list = spl_list_alloc();
	spl_list_append(spl_list, spl_hfile);
	spl_list_append(spl_list, spl_hopt);

	/* Append all found section headers. */
	for (i = 0; i < hfile->NumberOfSections; i++) {
		char cp[64];

		sprintf(cp, "IMAGE_SECTION_HEADER (%s)", (hsec + i)->Name);
		spl_list_append(spl_list, spl_alloc(cp, hsec_info_get(hsec + i)));

		if (!strcmp((const char *) (hsec + i)->Name, ".edata")) {
			struct pe_info pinfo = {bytes, hfile, hopt, hsec};
			IMAGE_EXPORT_DIRECTORY *edir;

			edir = hsec_dd_get(&pinfo, ".edata");
			if (!edir)
				continue;

			spl_list_append(spl_list, spl_alloc("IMAGE_EXPORT_DIRECTORY",
				hsec_ex_dd_info_get(edir)));
		}
	}
	
	spl_list_draw(spl_list);

	while (!quit) {
 		bool mouse_press_skip = false;
		bool update = false;
		int x;
		int y;
		int wheel_state;
		
		if (!buf_compare(buf_bas, buf_bas_old)) {
			buf_copy(buf_bas_old, buf_bas);
			
			update |= ~update;
		}

		if (!buf_compare(buf_rsv, buf_rsv_old)) {
			buf_copy(buf_rsv_old, buf_rsv);

			buf_rsv->rsv_lines = buf_measure(buf_rsv);
			
			if (wheel_pos > buf_rsv->rsv_lines)
				wheel_pos = buf_rsv->rsv_lines;
				
			update |= ~update;
		}

		if (~(update ^= 1) || ~(wheel ^= 1)) {
			if (wheel_pos > BUF_BAS_SIZE_Y)
				buf_draw(buf_rsv, wheel_pos, 0);
			else if (wheel_pos > 0) {
				buf_draw(buf_rsv, 0, wheel_pos);
				buf_draw(buf_bas, wheel_pos, 0);
			}
			else
				buf_draw(buf_bas, 0, 0);
		}

		ReadConsoleInput(hin, &ev_buf, 1, &(DWORD) {0});
		
		switch (ev_buf.EventType) {
		case MOUSE_EVENT:
			x = ev_buf.Event.MouseEvent.dwMousePosition.X;
			y = ev_buf.Event.MouseEvent.dwMousePosition.Y + wheel_pos;
			
			switch (ev_buf.Event.MouseEvent.dwEventFlags) {	
			case MOUSE_MOVED:
				spl_list_emit_scroll(spl_list, x, y);
				
				mouse_press_skip = true;
				
				break;
			case MOUSE_WHEELED:
				wheel_state = (int) ev_buf.Event.MouseEvent.dwButtonState;
				if (wheel_state > 0) {
					if ((wheel_pos -= WHEEL_OFFSET) < 0)
   						wheel_pos = 0;
				}
				else {
					if ((wheel_pos += WHEEL_OFFSET) > buf_rsv->rsv_lines)
						wheel_pos = buf_rsv->rsv_lines;
				}
				
				wheel = true;
				
				break;
			}
			
			switch (ev_buf.Event.MouseEvent.dwButtonState) {
			case FROM_LEFT_1ST_BUTTON_PRESSED:
				if (mouse_press_skip)
					break;
				
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
	
	fclose(flog);
	CloseHandle(file);
	free(bytes);
	buf_free(buf_bas_old);
	buf_free(buf_bas);
	buf_free(buf_rsv_old);
	buf_free(buf_rsv);
	spl_list_free(spl_list);
	
	return 0;	
}
