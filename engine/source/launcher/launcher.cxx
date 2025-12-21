export module mini.launcher;

import mini.debug;
import mini.platform;
import mini.engine;
import :static_init;

namespace mini::launcher {

Logger logger("Launcher");

} // namespace mini::launcher

namespace mini {

export void LaunchEngine(platform::Handle* handle)
{
    launcher::logger.Log("launching engine");

    launcher::StaticInitialize();
    launcher::logger.Log("static initialized");

    ENSURE(handle, "unsupported platform") return;
    ENSURE(Platform::Initialize(handle), "failed to initialize platform") {
        return;
    }
    launcher::logger.Log("platform initialized");

    Engine::Launch();

    launcher::StaticCleanup();
}

} // namespace mini