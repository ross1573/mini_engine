module;

#include "resource.h"

module mini.windows;

import mini.core;
import mini.platform;

namespace mini::windows {

Window::Window()
    : m_windowHandle(nullptr)
    , m_rect{}
    , m_isShowing(false)
    , m_state(Default)
{
    auto className = options::name;
    auto titleName = options::title;
    m_rect = RectInt(options::x, options::y, options::width, options::height);

    HINSTANCE instance = interface->GetHandle()->GetHINSTANCE();
    uint32 styleEx = WS_EX_APPWINDOW;
    uint32 style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;

    if (options::resizableWindow) {
        style |= WS_THICKFRAME;
    }

    m_windowHandle = CreateWindowExA(styleEx, className, titleName, style, m_rect.x, m_rect.y,
                                     m_rect.width, m_rect.height, nullptr, nullptr, instance,
                                     nullptr);

    SetMenu(m_windowHandle, 0);
}

bool Window::Valid() const noexcept
{
    return m_windowHandle != nullptr;
}

void Window::Resize(RectInt const& rect)
{
    m_rect = rect;
    SetWindowPos(m_windowHandle, nullptr, m_rect.x, m_rect.y, m_rect.width, m_rect.height, 0);
}

void Window::Minimize()
{
    if (IsMinimized()) {
        return;
    }

    ShowWindow(m_windowHandle, SW_MINIMIZE);
    m_state = Minimized;
}

void Window::Maximize()
{
    if (IsMaximized()) {
        return;
    }

    ShowWindow(m_windowHandle, SW_MAXIMIZE);
    m_state = Maximized;
}

void Window::Show()
{
    if (m_isShowing) {
        return;
    }

    ShowWindow(m_windowHandle, SW_SHOWDEFAULT);
    m_isShowing = true;
}

void Window::Hide()
{
    if (!m_isShowing) {
        return;
    }

    ShowWindow(m_windowHandle, SW_HIDE);
    m_isShowing = false;
}

} // namespace mini::windows