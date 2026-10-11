module;

#include <cstdlib>

module mini.engine;

import mini.core;
import mini.platform;
import mini.graphics;
import :log;

namespace mini {

Engine::Engine()
    : m_running(false)
    , m_frameCount(1)
    , m_platform("mini.platform")
    , m_graphics("mini.graphics")
{
    ASSERT(instance == nullptr, "another instance of engine is created");
    instance = this;
}

Engine::~Engine() noexcept
{
    ASSERT(m_running == false, "engine is still running");
    instance = nullptr;
}

void Engine::launch()
{
    ENSURE(m_running == false, "engine is already running") return;
    ENSURE(m_graphics->loadModule(options::graphicsModule)) return;

    m_platform->window()->show();
    m_platform->pollEvents();

    Clock::TimePoint frameStart;
    Clock::TimePoint frameEnd;
    Nanoseconds duration;
    uint64 lastFrameCount = 0;

    for (m_running = true; m_running; ++m_frameCount) {
        frameStart = Clock::now();

        m_graphics->renderFrame();
        m_platform->pollEvents();

        frameEnd = Clock::now();
        duration += frameEnd - frameStart;

        Milliseconds milliseconds = durationCast<Milliseconds>(duration);
        if (milliseconds.count() >= 1000) {
            uint64 count = m_frameCount - lastFrameCount;
            float64 fmilli = durationCast<FloatMilliseconds>(duration).count();
            float64 fps = static_cast<float64>(count) / fmilli * 1000.0;
            engine::logInfo("fps: {:.2f}", fps);

            lastFrameCount = m_frameCount;
            duration = Nanoseconds(0);
        }
    }
}

void Engine::shutdown()
{
    m_running = false;
}

void Engine::quit()
{
    if (instance != nullptr) {
        instance->shutdown();
    }
}

void Engine::abort(String const& msg)
{
    if (instance == nullptr) {
        std::exit(-1);
        return;
    }

    Platform::alertError(msg);
    instance->m_running = false;
}

bool Engine::running() noexcept
{
    return instance != nullptr && instance->m_running;
}

uint64 Engine::frameCount() noexcept
{
    return instance == nullptr ? 0 : instance->m_frameCount;
}

} // namespace mini