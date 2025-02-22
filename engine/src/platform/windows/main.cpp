#include "mini_engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
#ifdef DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    mini::Engine::Launch();

#ifdef DEBUG
    ASSERT(_CrtCheckMemory());
#endif
    return TRUE;
}