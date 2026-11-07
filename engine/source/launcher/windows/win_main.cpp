#include <windows.h>

#if DEBUG
#  include <crtdbg.h>
#endif

import mini.core;
import mini.launcher;

extern "C" int __stdcall WinMain(HINSTANCE /*unused*/, HINSTANCE /*unused*/, char* /*unused*/, int /*unused*/)
{
#if DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    mini::launch();

#if DEBUG
    ASSERT(_CrtCheckMemory());
#endif
    return 0;
}