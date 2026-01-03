export module mini.engine;

import mini.core;
import mini.platform;
import mini.graphics;

namespace mini {

export class Engine;

} // namespace mini

namespace mini::engine {

export class ENGINE_API Interface final : public Module::Interface {
private:
    Module m_Platform;
    Module m_Graphics;

    UniquePtr<Engine> m_Engine;

public:
    Interface();
    ~Interface() noexcept final;

    void Launch();
    void Shutdown();

    Engine* GetEngine() noexcept { return m_Engine.Get(); }

protected:
    bool Initialize() final;
};

ENGINE_API Interface* interface = nullptr;

} // namespace mini::engine

namespace mini {

class ENGINE_API Engine {
private:
    bool m_Running;
    platform::Interface* m_Platform;
    graphics::Interface* m_Graphics;

    friend class engine::Interface;

public:
    ~Engine();

    void Run();

    static void Quit();
    static void Abort(String const& = "");

    static bool IsRunning();

private:
    Engine(platform::Interface*, graphics::Interface*);
};

inline bool Engine::IsRunning()
{
    Engine* engine = engine::interface->GetEngine();
    return engine != nullptr && engine->m_Running;
}

} // namespace mini