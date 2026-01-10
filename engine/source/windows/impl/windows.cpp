module mini.windows;

import mini.platform;
import :handle;
import :window;

namespace mini {

Windows::Windows() noexcept
{
    windows::interface = this;
}

Windows::~Windows() noexcept
{
    windows::interface = nullptr;
}

windows::Handle* Windows::GetHandle() noexcept
{
    return static_cast<windows::Handle*>(m_Platform->GetHandle());
}

windows::Window* Windows::GetWindow() noexcept
{
    return static_cast<windows::Window*>(m_Platform->GetWindow());
}

platform::Handle* Windows::CreateHandle()
{
    return new windows::Handle();
}

platform::Window* Windows::CreateWindow()
{
    return new windows::Window();
}

} // namespace mini
