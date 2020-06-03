#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <windows.h>

void assert(HWND winh, const char *func_name);
CHAR_INFO * data_alloc(const char *data, WORD attrs);

#endif /* UTILS_H */
