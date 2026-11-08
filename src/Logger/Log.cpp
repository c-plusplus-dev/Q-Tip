//
// Created by Ayaan on 2026-11-08.
//

#include "Q-Tip/Logger/Log.h"

#ifdef _WIN32
    #define NOMINMAX
    #include <windows.h>
    #ifdef DELETE
    #undef DELETE
    #endif
    #ifdef OUT
    #undef OUT
    #endif
#endif

namespace Detail {

#ifdef _WIN32
    bool terminalSupportsANSI() {
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);

        if (output == INVALID_HANDLE_VALUE || output == nullptr) {
            return false;
        }

        DWORD mode = 0;

        if (!GetConsoleMode(output, &mode)) {
            return false;
        }

        return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
    }
#endif

}
