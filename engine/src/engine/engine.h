#pragma once

#include "container/string.h"

namespace mini
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
    static bool IsRunning();

    static void Launch();
    static void Quit();
    static void Abort(String const& = "");
};

} // namespace mini
