export module mini.engine;

import mini.core;

namespace mini {

export class Engine;

ENGINE_API UniquePtr<Engine> g_Engine;

class ENGINE_API Engine {
public:
    typedef void (*CallbackFunc)();

private:
    bool m_Running;

    Array<CallbackFunc> m_QuitCallback;
    Array<CallbackFunc> m_ExitCallback;

private:
    Engine();

    bool Initialize();
    void Shutdown();

public:
    ~Engine();

    static void Launch();
    static void Quit();
    static void Abort(String const& = "");

    static void AtQuit(CallbackFunc);
    static void AtExit(CallbackFunc);

    inline static bool IsRunning() { return g_Engine.Get() && g_Engine->m_Running; }
};

} // namespace mini
