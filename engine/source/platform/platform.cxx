export module mini.platform;

import mini.core;
export import :window;
export import :handle;

PLATFORM_API mini::platform::Handle* g_Handle = nullptr;
PLATFORM_API mini::platform::Window* g_Window = nullptr;

export namespace mini
{

class PLATFORM_API Platform
{
    typedef mini::platform::Handle Handle;
    typedef mini::platform::Window Window;

    friend class Engine;
    friend void LaunchEngine(Handle*);

public:
    template <typename T = Handle> static T* GetHandle() noexcept;
    template <typename T = Window> static T* GetWindow() noexcept;

private:
    Platform() = default;

    static bool Initialize(Handle*);
    static void Shutdown();
};

template <typename T>
inline T* Platform::GetHandle() noexcept
{
    return static_cast<T*>(g_Handle);
}

template <typename T>
inline T* Platform::GetWindow() noexcept
{
    return static_cast<T*>(g_Window);
}

} // namespace mini