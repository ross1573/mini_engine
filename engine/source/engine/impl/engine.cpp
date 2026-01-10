module;

#include <cstdlib>

module mini.engine;

import mini.core;
import mini.platform;
import mini.graphics;

namespace mini {

Engine::Engine()
    : m_Platform("platform")
    , m_Graphics("graphics")
    , m_Running(false)
{
    ASSERT(engine == nullptr, "another instance of engine is created");
    engine = this;
}

Engine::~Engine() noexcept
{
    ASSERT(m_Running == false, "engine is still running");
    engine = nullptr;
}

void Engine::Launch()
{
    ENSURE(m_Running == false, "engine is already running") return;

    m_Platform->GetWindow()->Show();
    m_Platform->PollEvents();

    m_Running = true;
    while (m_Running) {
        m_Graphics->BeginFrame();
        {
            platform::Window* window = m_Platform->GetWindow();
            RectInt windowSize = window->GetSize();
            Rect windowRect(windowSize);

            graphics::RenderContext* renderContext = m_Graphics->GetRenderContext();
            renderContext->SetViewport(windowRect, 0.1f, 100.f);
            renderContext->SetScissorRect(windowSize);
        }
        m_Graphics->EndFrame();

        m_Platform->PollEvents();
    }
}

void Engine::Shutdown()
{
    m_Running = false;
}

void Engine::Quit()
{
    if (engine != nullptr) {
        engine->Shutdown();
    }
}

void Engine::Abort(String const& msg)
{
    Platform::AlertError(msg);
    memory::DestructAt(&msg);

    engine->m_Running = false;
    engine = nullptr;

    std::exit(-1);
}

bool Engine::IsRunning() noexcept
{
    return engine != nullptr && engine->m_Running;
}

} // namespace mini