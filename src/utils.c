#include "utils.h"

void
assert(HWND winh, const char *func_name)
{
    DWORD msg_id = GetLastError();
    LPSTR msg_buf = NULL;
    char title[256];

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        msg_id,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR) &msg_buf,
        0,
        NULL);

    sprintf(title, "0x%lx (%s)", msg_id, func_name);
    MessageBox(winh, msg_buf, title, MB_OK | MB_ICONERROR);
    LocalFree(msg_buf);

    exit(EXIT_FAILURE);
}

CHAR_INFO *
data_alloc(const char *data, WORD attrs)
{
    size_t size = strlen(data);
    CHAR_INFO *ci = calloc(sizeof(CHAR_INFO), size);
    unsigned int i;

    for (i = 0; i < size; i++) {
        ci[i].Char.AsciiChar = data[i];
        ci[i].Attributes = attrs;
    }

    return ci;
}
