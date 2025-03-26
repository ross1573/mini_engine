module;

#include "assertion.h"

export module mini.launcher;

import mini.platform;
import mini.engine;

export namespace mini {

void LaunchEngine(platform::Handle* handle)
{
    ENSURE(handle, "unsupported platform") return;
    ENSURE(Platform::Initialize(handle), "failed to initialize platform") return;

    Engine::Launch();
}

} // namespace mini