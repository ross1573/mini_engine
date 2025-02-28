export module mini.platform;

import mini.core;
export import :window;
export import :handle;

export namespace mini
{

class Platform
{
    typedef mini::platform::Handle Handle;
    typedef mini::platform::Window Window;

    friend class Engine;

private:
    static Handle* m_Handle;
    static Window* m_Window;

private:
    Platform() = default;

    static bool Initialize();
    static void Shutdown();

public:
    template <typename T = Handle> static T* GetHandle() noexcept;
    template <typename T = Window> static T* GetWindow() noexcept;
};

template <typename T>
inline T* Platform::GetHandle() noexcept
{
    return static_cast<T*>(m_Handle);
}

template <typename T>
inline T* Platform::GetWindow() noexcept
{
    return static_cast<T*>(m_Window);
}

} // namespace mini