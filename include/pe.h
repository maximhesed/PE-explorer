#ifndef PE_H
#define PE_H

#include <windows.h>

#include "log.h"

struct pe_info {
	LPVOID bytes;
	IMAGE_FILE_HEADER *hfile;
	IMAGE_OPTIONAL_HEADER *hopt;
	IMAGE_SECTION_HEADER *hsec;
};

DWORD sign_get(LPVOID bytes);
LPVOID hfile_get(LPVOID addr);
LPVOID hopt_get(LPVOID addr);
LPVOID hsec_get(LPVOID addr);
char * hfile_info_get(IMAGE_FILE_HEADER *hfile);
char * hopt_info_get(IMAGE_OPTIONAL_HEADER *hopt);
char * hsec_info_get(IMAGE_SECTION_HEADER *hsec);
int hsec_offset_get(struct pe_info *pinfo, const char *sname);
LPVOID hsec_dd_get(struct pe_info *pinfo, const char *sname);

#endif /* PE_H */
