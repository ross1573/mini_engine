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

windows::Handle* Windows::handle() noexcept
{
    return static_cast<windows::Handle*>(m_platform->handle());
}

windows::Window* Windows::window() noexcept
{
    return static_cast<windows::Window*>(m_platform->window());
}

platform::Handle* Windows::createHandle()
{
    return new windows::Handle();
}

platform::Window* Windows::createWindow()
{
    return new windows::Window();
}

} // namespace mini
