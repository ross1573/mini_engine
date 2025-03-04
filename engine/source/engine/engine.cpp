module;

#include <cstdlib>
#include "assertion.h"

module mini.engine;

import mini.core;
import mini.platform;
import mini.graphics;

#include "option.h"

namespace mini
{

Engine::Engine()
    : m_Running(false)
    , m_QuitCallback()
    , m_ShutdownCallback()
{
}

Engine::~Engine()
{
    m_Running = false;
}

bool Engine::Initialize()
{
    platform::Handle* platformHandle = Platform::GetHandle();
    graphics::Device* device = platformHandle->CreateGraphicDevice(options::graphicsAPI);

    ASSERT(device, "failed to create graphic device");
    VERIFY(Graphics::Initialize(device), "failed to initialize graphics");

    m_Running = true;
    return true;
}

void Engine::Shutdown()
{
    Graphics::Shutdown();
    Platform::Shutdown();

    for (auto func : g_Engine->m_ShutdownCallback)
    {
        func();
    }

    DELETE(g_Engine);
}

void Engine::Launch()
{
    ASSERT(g_Engine == nullptr, "another instance of engine is running");
    g_Engine = new Engine();

    if (!g_Engine->Initialize())
    {
        return;
    }

    Platform::GetWindow()->Show();
    Platform::GetHandle()->PollEvents();

    while (g_Engine->m_Running)
    {
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
    for (auto func : g_Engine->m_QuitCallback)
    {
        func();
    }

    if (g_Engine != nullptr)
    {
        g_Engine->m_Running = false;
    }
}

void Engine::Abort(String msg)
{
    Platform::GetWindow()->DialogCritical(msg);

    if (g_Engine != nullptr)
    {
        g_Engine->m_Running = false;
        g_Engine->Shutdown();
    }

    std::exit(-1);
}

void Engine::AtQuit(CallbackFunc func)
{
    g_Engine->m_QuitCallback.Push(func);
}

void Engine::AtShutdown(CallbackFunc func)
{
    g_Engine->m_ShutdownCallback.Push(func);
}

} // namespace mini