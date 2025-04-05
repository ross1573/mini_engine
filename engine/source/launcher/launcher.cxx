module;

#include "assertion.h"

export module mini.launcher;

import mini.core;
import mini.platform;
import mini.engine;

export namespace mini {

void LaunchEngine(platform::Handle* handle)
{
    log::Info("launching engine...");

    ENSURE(handle, "unsupported platform") return;
    ENSURE(Platform::Initialize(handle), "failed to initialize platform") {
        return;
    }
    log::Info("platform initialized");

    Engine::Launch();
}

} // namespace mini