export module mini.engine;

import mini.core;

namespace mini { class Engine; }

ENGINE_API mini::Engine* g_Engine; 

export namespace mini
{

class ENGINE_API Engine
{
public:
    typedef void (*CallbackFunc)();

private:
    bool m_Running;

    Array<CallbackFunc> m_QuitCallback;
    Array<CallbackFunc> m_ShutdownCallback;

private:
    Engine();
    ~Engine();
 
    bool Initialize();
    void Shutdown();

public:
    static void Launch();
    static void Quit();
    static void Abort(String = "");

    static void AtQuit(CallbackFunc);
    static void AtShutdown(CallbackFunc);

    inline static bool IsRunning() { return g_Engine && g_Engine->m_Running; }
};

} // namespace mini
