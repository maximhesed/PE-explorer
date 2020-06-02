#include "pe.h"

static const size_t SIZE_OF_NT_SIGNATURE = sizeof(DWORD);

static LPVOID
sig_nt(LPVOID addr, int offset) {
	return (LPVOID) ((BYTE *) addr + ((PIMAGE_DOS_HEADER) addr)->e_lfanew +
		offset);
}

void 
assert(char *func_name) {
	DWORD msg_id = GetLastError();
	LPSTR msg_buf = NULL;
	
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
			| FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
		msg_id,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR) &msg_buf,
		0,
		NULL);
	
	fprintf(flog, "%s(): %s", func_name, msg_buf);
	
	LocalFree(msg_buf);
	
	exit(EXIT_FAILURE);
}

DWORD
sign_get(LPVOID bytes) {
	if (*(USHORT *) bytes == IMAGE_DOS_SIGNATURE) {
		if (LOWORD (*(DWORD *) sig_nt(bytes, 0)) == IMAGE_OS2_SIGNATURE 
				|| LOWORD (*(DWORD *) sig_nt(bytes, 0)) 
				== IMAGE_OS2_SIGNATURE_LE)
			return (DWORD) LOWORD (*(DWORD *) sig_nt(bytes, 0));
		else if (*(DWORD *) sig_nt(bytes, 0) == IMAGE_NT_SIGNATURE)
			return IMAGE_NT_SIGNATURE;
		else
			return IMAGE_DOS_SIGNATURE;
	}
	
	return 0;
}

LPVOID hfile_get(LPVOID addr) {
	return (PIMAGE_FILE_HEADER) sig_nt(addr, SIZE_OF_NT_SIGNATURE);
}

LPVOID hopt_get(LPVOID addr) {
	return (PIMAGE_OPTIONAL_HEADER) sig_nt(addr, SIZE_OF_NT_SIGNATURE +
		sizeof(IMAGE_FILE_HEADER));
}

LPVOID hsec_get(LPVOID addr) {
	return (PIMAGE_SECTION_HEADER) sig_nt(addr, SIZE_OF_NT_SIGNATURE +
		sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_OPTIONAL_HEADER));
}

char *
hfile_info_get(void) {
	char *data = calloc(sizeof(char), 512);
	
	sprintf(data,
		"Machine: %d\n"
		"NumberOfSections: %d\n"
		"TimeDateStamp: %lu\n"
		"PointerToSymbolTable: 0x%lx\n" 
		"NumberOfSymbols: %lu\n"
		"SizeOfOptionalHeader: %d\n"
		"Characteristics: 0x%x\n",
		hfile->Machine, 
		hfile->NumberOfSections, 
		hfile->TimeDateStamp,
		hfile->PointerToSymbolTable,
		hfile->NumberOfSymbols,
		hfile->SizeOfOptionalHeader,
		hfile->Characteristics);
		
	return data;
}

char *
hopt_info_get(void) {
	char *data = calloc(sizeof(char), 8192);

	sprintf(data,
		"Magic: %d\n"
		"MajorLinkerVersion: %u\n"
		"MinorLinkerVersion: %u\n"
		"SizeOfCode: %lu\n"
		"AddressOfEntryPoint: 0x%lx\n" 
		"BaseOfCode: 0x%lx\n"
		"BaseOfData: 0x%lx\n"
		"ImageBase: 0x%lx\n"
		"SectionAlignment: %lu\n"
		"FileAlignment: %lu\n"
		"MajorOperatingSystemVersion: %u\n"
		"MinorOperatingSystemVersion: %u\n"
		"MajorImageVersion: %u\n"
		"MinorImageVersion: %u\n"
		"SizeOfImage: %lu\n"
		"SizeOfHeaders: %lu\n"
		"CheckSum: %lu\n"
		"Sybsystem: %u\n"
		"DllCharacteristics: 0x%x\n"
		"SizeOfStackReserve: %lu\n"
	    "SizeOfStackCommit: %lu\n"
	    "SizeOfHeapReserve: %lu\n"
	    "SizeOfHeapCommit: %lu\n"
	    "LoaderFlags: %lu\n"
	    "NumberOfRvaAndSizes: %lu\n",
	    hopt->Magic,
	    hopt->MajorLinkerVersion,
	    hopt->MinorLinkerVersion,
	    hopt->SizeOfCode,
	    hopt->AddressOfEntryPoint,
	    hopt->BaseOfCode,
	    hopt->BaseOfData,
	    hopt->ImageBase,
	    hopt->SectionAlignment,
	    hopt->FileAlignment,
	    hopt->MajorOperatingSystemVersion,
	    hopt->MinorOperatingSystemVersion,
	    hopt->MajorImageVersion,
	    hopt->MinorImageVersion,
	    hopt->SizeOfImage,
	    hopt->SizeOfHeaders,
	    hopt->CheckSum,
	    hopt->Subsystem,
	    hopt->DllCharacteristics,
	    hopt->SizeOfStackReserve,
	    hopt->SizeOfStackCommit,
	    hopt->SizeOfHeapReserve,
	    hopt->SizeOfHeapCommit,
	    hopt->LoaderFlags,
	    hopt->NumberOfRvaAndSizes);

	return data;
}

char *
hsec_info_get(IMAGE_SECTION_HEADER *sec) {
	char *data = calloc(sizeof(char), 512);
	
	sprintf(data,
		"VirtualAddress: 0x%lx\n"
		"SizeOfRawData: %lu\n"
		"PointerToRawData: 0x%lx\n"
		"Characteristics: 0x%lx\n",
		sec->VirtualAddress,
		sec->SizeOfRawData,
		sec->PointerToRawData,
		sec->Characteristics);
		
	return data;
}

int
hsec_offset_get(const char *sname) {
	int i;
	
	for (i = 0; i < hfile->NumberOfSections; i++) {
		if (!strcmp((const char *) (hsec + i)->Name, sname))
			return i;
	}
	
	return -1;
}

LPVOID
hsec_dd_get(LPVOID bytes, const char *sname) {
	PIMAGE_SECTION_HEADER hsec_local;
	int offset;
	
	offset = hsec_offset_get(sname);
	if (offset == -1)
		return NULL;
	
	hsec_local = hsec + offset;
	
	return (LPVOID) ((DWORD) bytes + hsec_local->PointerToRawData 
		+ hopt->DataDirectory[offset].VirtualAddress);
}
