#include <windows.h>

import mini.launcher;
import mini.windows;

int APIENTRY wWinMain(_In_ HINSTANCE inst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
#ifdef DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    mini::MiniLaunch(new mini::windows::Handle(inst));

#ifdef DEBUG
    ASSERT(_CrtCheckMemory());
#endif
    return TRUE;
}