export module mini.launcher;

import mini.core;
import mini.platform;
import mini.engine;

namespace mini {

export void LaunchEngine(platform::Handle* handle)
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