module;

#include <cstdlib>

module mini.engine;

import mini.core;
import mini.platform;
import mini.graphics;

namespace mini {

Engine::Engine()
    : m_running(false)
    , m_frameCount(0)
    , m_platform("mini.platform")
    , m_graphics("mini.graphics")
{
    ASSERT(engine == nullptr, "another instance of engine is created");
    engine = this;
}

Engine::~Engine() noexcept
{
    ASSERT(m_running == false, "engine is still running");
    engine = nullptr;
}

void Engine::Launch()
{
    ENSURE(m_running == false, "engine is already running") return;
    ENSURE(m_graphics->LoadModule(options::graphicsModule)) return;

    m_platform->GetWindow()->Show();
    m_platform->PollEvents();

    m_running = true;
    for (; m_running; ++m_frameCount) {
        m_graphics->RenderFrame();
        m_platform->PollEvents();
    }
}

void Engine::Shutdown()
{
    m_running = false;
}

void Engine::Quit()
{
    if (engine != nullptr) {
        engine->Shutdown();
    }
}

void Engine::Abort(String const& msg)
{
    if (engine == nullptr) {
        std::exit(-1);
        return;
    }

    Platform::AlertError(msg);
    engine->m_running = false;
}

bool Engine::Running() noexcept
{
    return engine != nullptr && engine->m_running;
}

uint64 Engine::FrameCount() noexcept
{
    return engine == nullptr ? 0 : engine->m_frameCount;
}

} // namespace mini