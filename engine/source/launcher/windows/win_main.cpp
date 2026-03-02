#if DEBUG
#  include <crtdbg.h>
#endif

import mini.launcher;

extern "C" int __stdcall wWinMain(void*, void*, wchar_t*, int)
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