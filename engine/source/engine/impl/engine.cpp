module;

#include <cstdlib>

module mini.engine;

import mini.memory;
import mini.math;
import mini.string;
import mini.platform;
import mini.graphics;

namespace mini {

Engine::Engine()
    : m_Running(false)
    , m_QuitCallback()
    , m_ExitCallback()
{
}

Engine::~Engine()
{
    m_Running = false;
}

bool Engine::Initialize()
{
    ENSURE(Graphics::Initialize(), "failed to initialize graphics") return false;

    m_Running = true;
    return true;
}

void Engine::Shutdown()
{
    Graphics::Shutdown();
    Platform::Shutdown();

    for (auto func : g_Engine->m_ExitCallback) {
        func();
    }

    g_Engine.Reset();
}

void Engine::Launch()
{
    ASSERT(g_Engine == nullptr, "another instance of engine is running");
    g_Engine = UniquePtr(new Engine());

    if (!g_Engine->Initialize()) {
        return;
    }

    Platform::GetWindow()->Show();
    Platform::GetHandle()->PollEvents();

    while (g_Engine->m_Running) {
        Graphics::BeginFrame();
        {
            RectInt windowSize = Platform::GetWindow()->GetSize();
            Rect windowRect(windowSize);

            Graphics::GetRenderContext()->SetViewport(windowRect, 0.1f, 100.f);
            Graphics::GetRenderContext()->SetScissorRect(windowSize);
        }
        Graphics::EndFrame();

        Platform::GetHandle()->PollEvents();
    }

    g_Engine->Shutdown();
}

void Engine::Quit()
{
    for (auto func : g_Engine->m_QuitCallback) {
        func();
    }
    g_Engine->m_QuitCallback.Clear();

    if (g_Engine != nullptr) {
        g_Engine->m_Running = false;
    }
}

void Engine::Abort(String const& msg)
{
    Platform::GetWindow()->DialogCritical(msg);

    if (g_Engine != nullptr) {
        g_Engine->m_Running = false;
        g_Engine->Shutdown();
    }

    memory::DestructAt(&msg);
    std::exit(-1);
}

void Engine::AtQuit(CallbackFunc func)
{
    g_Engine->m_QuitCallback.Push(func);
}

void Engine::AtExit(CallbackFunc func)
{
    g_Engine->m_ExitCallback.Push(func);
}

} // namespace mini