#pragma once

#include "core/utility.h"

namespace mini::platform
{
class Handle;
class Window;
}

namespace mini
{

class Platform
{
    typedef platform::Handle Handle;
    typedef platform::Window Window;

    friend class Engine;

private:
    static inline Handle* m_Handle = nullptr;
    static inline Window* m_Window = nullptr;

private:
    Platform() = default;

    static bool Initialize();
    static void Shutdown();

public:
    template <typename T = Handle> static T* GetHandle() noexcept;
    template <typename T = Window> static T* GetWindow() noexcept;
};

template <typename T>
[[force_inline]] T* Platform::GetHandle() noexcept
{
    return static_cast<T*>(m_Handle);
}

template <typename T>
[[force_inline]] T* Platform::GetWindow() noexcept
{
    return static_cast<T*>(m_Window);
}

} // namespace mini