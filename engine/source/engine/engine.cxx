module;

export module mini.engine;

import mini.core;

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
    static void Abort(String = "");

    inline static bool IsRunning() { return m_Engine && m_Engine->m_Running; }
};

} // namespace mini
