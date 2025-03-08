export module mini.platform;

import mini.core;
export import :window;
export import :handle;

namespace mini
{

PLATFORM_API UniquePtr<platform::Handle> g_Handle;
PLATFORM_API UniquePtr<platform::Window> g_Window;

export class PLATFORM_API Platform
{
public:
    typedef mini::platform::Handle Handle;
    typedef mini::platform::Window Window;

public:
    inline static Handle* GetHandle() noexcept { return g_Handle.Get(); }
    inline static Window* GetWindow() noexcept { return g_Window.Get(); }

public:
    Platform() = default;

    static bool Initialize(Handle*);
    static void Shutdown();
};

} // namespace mini