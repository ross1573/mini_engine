export module mini.launcher;

import mini.core;
import mini.engine;
import :static_init;
import :log;

namespace mini {

void launchEngine()
{
    launcher::logInfo("launching engine");

    Module<Engine> engine("mini.engine");

    engine->launch();
    engine.release();
}

export void launch()
{
    launcher::staticInitialize();
    {
        Module<Core> core("mini.core");

        launchEngine();

        [[maybe_unused]] size_t remaining = core->moduleCount();
        ASSERT(remaining == 1, "{} module remaining", remaining);
    }
    launcher::staticCleanup();
}

} // namespace mini