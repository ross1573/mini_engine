export module mini.launcher;

import mini.core;
import mini.engine;
import :static_init;

namespace mini::launcher {

Logger logger("Launcher");

} // namespace mini::launcher

namespace mini {

export void LaunchEngine()
{
    launcher::logger.Log("launching engine");

    launcher::StaticInitialize();
    launcher::logger.Log("static initialized");

    Module<Engine> engine("engine");
    engine->Launch();
    engine.Release();

    launcher::StaticCleanup();
}

} // namespace mini