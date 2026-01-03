module;

#include "resource.h"

module mini.windows;

import mini.core;
import mini.platform;

namespace mini::windows {

Window::Window()
    : m_WindowHandle(nullptr)
    , m_Rect{}
    , m_IsShowing(false)
    , m_State(Default)
{
    auto className = options::name;
    auto titleName = options::title;
    m_Rect = RectInt(options::x, options::y, options::width, options::height);

    HINSTANCE instance = ((Handle*)Platform::GetHandle())->GetHINSTANCE();
    uint32 styleEx = WS_EX_APPWINDOW;
    uint32 style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;

    if (options::resizableWindow) {
        style |= WS_THICKFRAME;
    }

    m_WindowHandle = CreateWindowExA(styleEx, className, titleName, style, m_Rect.x, m_Rect.y,
                                     m_Rect.width, m_Rect.height, nullptr, nullptr, instance,
                                     nullptr);

    SetMenu(m_WindowHandle, 0);
}

bool Window::IsValid() const noexcept
{
    return m_WindowHandle != nullptr;
}

void Window::DialogCritical(String const& msg)
{
    MessageBoxA(m_WindowHandle, msg.Data(), nullptr, MB_ICONERROR | MB_OK);
}

void Window::Resize(RectInt const& rect)
{
    m_Rect = rect;
    SetWindowPos(m_WindowHandle, nullptr, m_Rect.x, m_Rect.y, m_Rect.width, m_Rect.height, 0);
}

void Window::Minimize()
{
    if (IsMinimized()) {
        return;
    }

    ShowWindow(m_WindowHandle, SW_MINIMIZE);
    m_State = Minimized;
}

void Window::Maximize()
{
    if (IsMaximized()) {
        return;
    }

    ShowWindow(m_WindowHandle, SW_MAXIMIZE);
    m_State = Maximized;
}

void Window::Show()
{
    if (m_IsShowing) {
        return;
    }

    ShowWindow(m_WindowHandle, SW_SHOWDEFAULT);
    m_IsShowing = true;
}

void Window::Hide()
{
    if (!m_IsShowing) {
        return;
    }

    ShowWindow(m_WindowHandle, SW_HIDE);
    m_IsShowing = false;
}

} // namespace mini::windows