#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

#if DEBUG
#  include <crtdbg.h>
#endif

import mini.launcher;

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
#if DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    mini::LaunchEngine();

#if DEBUG
    ASSERT(_CrtCheckMemory());
#endif
    return 0;
}