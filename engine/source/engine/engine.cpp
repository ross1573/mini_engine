module;

#include <cstdlib>
#include "core/assert.h"

module mini.engine;

import mini.core;
import mini.platform;
import mini.graphics;

#include "core/option.h"

namespace mini
{

Engine::Engine()
    : m_Running(false)
    , m_QuitCallback()
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

    while (g_Engine->m_Running)
    {
        if (!Platform::GetHandle()->PollEvents())
        {
            g_Engine->m_Running = false;
            break;
        }

        Graphics::BeginFrame();
        {
            RectInt windowSize = Platform::GetWindow()->GetSize();
            Rect windowRect(windowSize);

            Graphics::GetRenderContext()->SetViewport(windowRect, 0.1f, 100.f);
            Graphics::GetRenderContext()->SetScissorRect(windowSize);
        }
        Graphics::EndFrame();
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

void Engine::AtQuit(QuitFunc func)
{
    g_Engine->m_QuitCallback.Push(func);
}

} // namespace mini