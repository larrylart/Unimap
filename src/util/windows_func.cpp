
#include <Windows.h>

// main header
#include "windows_func.h"

bool IsWin7OrLater() {
    DWORD version = GetVersion();
    DWORD major = (DWORD) (LOBYTE(LOWORD(version)));
    DWORD minor = (DWORD) (HIBYTE(LOWORD(version)));

    return (major > 6) || ((major == 6) && (minor >= 1));
}

