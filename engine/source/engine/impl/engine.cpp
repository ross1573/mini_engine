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
    ASSERT(g_engine == nullptr, "another instance of engine is created");
    g_engine = this;
}

Engine::~Engine() noexcept
{
    ASSERT(m_running == false, "engine is still running");
    g_engine = nullptr;
}

void Engine::Launch()
{
    ENSURE(m_running == false, "engine is already running") return;
    ENSURE(m_graphics->LoadModule(options::graphicsModule)) return;

    m_platform->GetWindow()->Show();
    m_platform->PollEvents();

    Clock::TimePoint frameStart;
    Clock::TimePoint frameEnd;
    Nanoseconds duration;
    uint64 lastFrameCount = 0;

    for (m_running = true; m_running; ++m_frameCount) {
        frameStart = Clock::Now();

        m_graphics->RenderFrame();
        m_platform->PollEvents();

        frameEnd = Clock::Now();
        duration += frameEnd - frameStart;

        Milliseconds milliseconds = DurationCast<Milliseconds>(duration);
        if (milliseconds.Count() >= 1000) {
            uint64 count = m_frameCount - lastFrameCount;
            float64 fmilli = DurationCast<FloatMilliseconds>(duration).Count();
            float64 fps = static_cast<float64>(count) / fmilli * 1000.0;
            engine::LogInfo("fps: {:.2f}", fps);

            lastFrameCount = m_frameCount;
            duration = Nanoseconds(0);
        }
    }
}

void Engine::Shutdown()
{
    m_running = false;
}

void Engine::Quit()
{
    if (g_engine != nullptr) {
        g_engine->Shutdown();
    }
}

void Engine::Abort(String const& msg)
{
    if (g_engine == nullptr) {
        std::exit(-1);
        return;
    }

    Platform::AlertError(msg);
    g_engine->m_running = false;
}

bool Engine::Running() noexcept
{
    return g_engine != nullptr && g_engine->m_running;
}

uint64 Engine::FrameCount() noexcept
{
    return g_engine == nullptr ? 0 : g_engine->m_frameCount;
}

} // namespace mini