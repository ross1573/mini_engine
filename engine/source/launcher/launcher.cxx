module;

#include "assertion.h"

export module mini.launcher;

import mini.platform;
import mini.engine;

export namespace mini {

void LaunchEngine(platform::Handle* handle)
{
    ASSERT(handle);
    VERIFY(Platform::Initialize(handle), "failed to initialize platform");

    Engine::Launch();
}

} // namespace mini