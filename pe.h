#ifndef PE
#define PE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "log.h"

void assert(char *func_name);
DWORD sign_get(LPVOID bytes);
LPVOID hfile_get(LPVOID addr);
LPVOID hopt_get(LPVOID addr);
LPVOID hsec_get(LPVOID addr);
char * hfile_info_get(void);
char * hopt_info_get(void);
char * hsec_info_get(IMAGE_SECTION_HEADER *sec);
int hsec_offset_get(const char *sname);
LPVOID hsec_dd_get(LPVOID bytes, const char *sname);

PIMAGE_FILE_HEADER hfile;
PIMAGE_OPTIONAL_HEADER hopt;
PIMAGE_SECTION_HEADER hsec;

#endif /* PE */
