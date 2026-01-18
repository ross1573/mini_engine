module;

#include <cstdlib>

module mini.engine;

import mini.core;
import mini.platform;
import mini.graphics;

namespace mini {

Engine::Engine()
    : m_running(false)
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

    Module<Platform> platform("platform");
    Module<Graphics> graphics("graphics");

    platform->GetWindow()->Show();
    platform->PollEvents();

    m_running = true;
    while (m_running) {
        graphics->BeginFrame();
        {
            platform::Window* window = platform->GetWindow();
            RectInt windowSize = window->GetSize();
            Rect windowRect(windowSize);

            graphics::RenderContext* renderContext = graphics->GetRenderContext();
            renderContext->SetViewport(windowRect, 0.1f, 100.f);
            renderContext->SetScissorRect(windowSize);
        }
        graphics->EndFrame();

        platform->PollEvents();
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

} // namespace mini