export module mini.launcher;

import mini.core;
import mini.engine;
import :static_init;
import :log;

namespace mini {

void LaunchEngine()
{
    launcher::LogInfo("launching engine");

    Module<Engine> engine("mini.engine");

    engine->Launch();
    engine.Release();
}

export void Launch()
{
    launcher::StaticInitialize();
    {
        Module<Core> core("mini.core");

        LaunchEngine();

        [[maybe_unused]] size_t remaining = core->ModuleCount();
        ASSERT(remaining == 1, "{} module remaining", remaining);
    }
    launcher::StaticCleanup();
}

} // namespace mini