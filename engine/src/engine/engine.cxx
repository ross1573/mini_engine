module;

export module mini.engine;

import mini.container;

export namespace mini
{

class Engine
{
private:
    static Engine* m_Engine;

    bool m_Running;

private:
    Engine();
    ~Engine();
 
    bool Initialize();
    void Shutdown();

public:
    static void Launch();
    static void Quit();
    static void Abort(String const& = "");

    inline static bool IsRunning() { return m_Engine && m_Engine->IsRunning(); }
};

} // namespace mini
