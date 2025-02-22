#include "mini_engine.h"
#include "engine/engine.h"

#ifdef TEST_BUILD
void TestMain();
#endif

namespace mini
{

Engine* Engine::m_Engine = nullptr;

Engine::Engine()
    : m_Running(false)
{
}

Engine::~Engine()
{    
    m_Running = false;
}

bool Engine::Initialize()
{
    VERIFY(Platform::Initialize(), "failed to initialize platform");

    platform::Handle* platformHandle = Platform::GetHandle();
    graphics::Device* device = platformHandle->CreateGraphicDevice(options::graphicsAPI);

    ASSERT(device, "failed to create graphic device");
    VERIFY(Graphics::Initialize(device), "failed to initialize graphics");

    m_Running = true;
    return true;
}

bool Engine::IsRunning()
{
    return m_Engine && m_Engine->IsRunning();
}

void Engine::Launch()
{
#ifdef TEST_BUILD
    TestMain();
    return;
#endif

    ASSERT(m_Engine == nullptr, "another instance of engine is running");
    m_Engine = new Engine();

    if (!m_Engine->Initialize())
    {
        return;
    }

    while (m_Engine->m_Running)
    {
        Platform::GetHandle()->PollEvents();

        if (!m_Engine->m_Running) break;

        Graphics::BeginFrame();
        {
            RectInt windowSize = Platform::GetWindow()->GetSize();
            Rect windowRect(windowSize);

            Graphics::GetRenderContext()->SetViewport(windowRect, 0.1f, 100.f);
            Graphics::GetRenderContext()->SetScissorRect(windowSize);
        }
        Graphics::EndFrame();
    }

    Graphics::Shutdown();
    Platform::Shutdown();

    DELETE(m_Engine);
}

void Engine::Quit()
{
    if (m_Engine != nullptr)
    {
        m_Engine->m_Running = false;
    }
}

void Engine::Abort(String const &msg)
{
    Quit();
    Platform::GetWindow()->DialogCritical(msg);
}

} // namespace mini