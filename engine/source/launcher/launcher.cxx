export module mini.launcher;

import mini.core;
import mini.platform;
import mini.engine;
import :static_init;

namespace mini {

export void LaunchEngine(platform::Handle* handle)
{
    launcher::StaticInitialize();

    log::Info("launching engine...");

    ENSURE(handle, "unsupported platform") return;
    ENSURE(Platform::Initialize(handle), "failed to initialize platform") {
        return;
    }
    log::Info("platform initialized");

    Engine::Launch();

    launcher::StaticCleanup();
}

} // namespace mini