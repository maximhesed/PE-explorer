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

DWORD sign_get(const LPVOID bytes);
LPVOID hfile_get(const LPVOID addr);
LPVOID hopt_get(const LPVOID addr);
LPVOID hsec_get(const LPVOID addr);
char * hfile_info_get(const IMAGE_FILE_HEADER *hfile);
char * hopt_info_get(const IMAGE_OPTIONAL_HEADER *hopt);
char * hsec_info_get(const IMAGE_SECTION_HEADER *hsec);
int hsec_offset_get(const struct pe_info *pinfo, const char *sname);
LPVOID hsec_dd_get(const struct pe_info *pinfo, const char *sname);

#endif /* PE_H */
