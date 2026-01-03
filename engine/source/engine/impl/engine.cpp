module;

#include <cstdlib>

module mini.engine;

import mini.core;
import mini.platform;
import mini.graphics;

namespace mini::engine {

Interface::Interface()
{
    interface = this;
}

Interface::~Interface() noexcept
{
    interface = nullptr;
}

bool Interface::Initialize()
{
    m_Platform.Load("platform");
    m_Graphics.Load("graphics");

    return m_Platform.IsValid() && m_Graphics.IsValid();
}

void Interface::Launch()
{
    ENSURE(m_Engine == nullptr, "another instance of engine is created") {
        return;
    }

    platform::Interface* platform = m_Platform.GetInterface<platform::Interface>();
    graphics::Interface* graphics = m_Graphics.GetInterface<graphics::Interface>();

    m_Engine = UniquePtr(new Engine(platform, graphics));

    platform->GetWindow()->Show();
    platform->PollEvents();

    m_Engine->Run();
}

void Interface::Shutdown()
{
    m_Engine->m_Running = false;
    m_Engine.Reset();

    std::exit(-1);
}

} // namespace mini::engine

namespace mini {

Engine::Engine(platform::Interface* platform, graphics::Interface* graphics)
    : m_Running(false)
    , m_Platform(platform)
    , m_Graphics(graphics)
{
}

Engine::~Engine()
{
    ENSURE(m_Running == false, "engine is still running") {}

    m_Platform = nullptr;
    m_Graphics = nullptr;
}

void Engine::Run()
{
    ENSURE(m_Running == false, "engine is already running") {
        return;
    }

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

void Engine::Quit()
{
    Engine* engine = engine::interface->GetEngine();
    if (engine != nullptr) {
        engine->m_Running = false;
    }
}

void Engine::Abort(String const& msg)
{
    Platform::AlertError(msg);
    memory::DestructAt(&msg);

    engine::interface->Shutdown();
}

} // namespace mini