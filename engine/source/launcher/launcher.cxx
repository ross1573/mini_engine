module;

#include "core/assert.h"

#ifdef TEST_BUILD
void TestMain();
#endif

export module mini.launcher;

import mini.platform;
import mini.graphics;
import mini.engine;

export namespace mini
{

void MiniLaunch(platform::Handle* handle)
{
#ifdef TEST_BUILD
    TestMain();
    return;
#endif 

    ASSERT(handle);
    VERIFY(Platform::Initialize(handle), "failed to initialize platform");
    
    Engine::Launch();
}

}