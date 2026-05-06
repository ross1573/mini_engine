export module mini.launcher;

import mini.core;
import mini.engine;
import :static_init;
import :log;

namespace mini {

export void LaunchEngine()
{
    launcher::LogInfo("launching engine");

    launcher::StaticInitialize();
    launcher::LogInfo("static initialized");

    Module<Core> core("core");
    Module<Engine> engine("engine");

    engine->Launch();
    engine.Release();

    launcher::StaticCleanup();
}

} // namespace mini