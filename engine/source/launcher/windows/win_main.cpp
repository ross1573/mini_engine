#include <windows.h>

#if DEBUG
#  include <crtdbg.h>

#  include "assertion.h"
#endif

import mini.launcher;
import mini.windows;

int APIENTRY wWinMain(_In_ HINSTANCE inst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
#if DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    mini::LaunchEngine(new mini::windows::Handle(inst));

#if DEBUG
    ASSERT(_CrtCheckMemory());
#endif
    return TRUE;
}