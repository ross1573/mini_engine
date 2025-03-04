module;

#include "resource/resource.h"
#include "assertion.h"

module mini.windows;

import mini.core;
import mini.platform;

#include "option.h"

namespace mini::windows
{

Window::Window()
    : m_WindowHandle(nullptr)
    , m_Rect{}
    , m_IsShowing(false)
{
}

bool Window::Initialize()
{
    auto className = mini::options::name;
    auto titleName = mini::options::title;
    m_Rect = mini::options::windowRect;

    HINSTANCE instance = Platform::GetHandle<Handle>()->GetHINSTANCE();
    uint32 styleEx = WS_EX_APPWINDOW;
    uint32 style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;

    if (mini::options::resizableWindow)
    {
        style |= WS_THICKFRAME;
    }

    m_WindowHandle = CreateWindowExA(styleEx,
                                     className,
                                     titleName,
                                     style,
                                     m_Rect.x, m_Rect.y,
                                     m_Rect.width, m_Rect.height,
                                     nullptr,
                                     nullptr,
                                     instance,
                                     nullptr);

    VERIFY(m_WindowHandle, "failed to create window");

    SetMenu(m_WindowHandle, 0);

    return true;
}

void Window::DialogCritical(String const& msg)
{
    MessageBoxA(m_WindowHandle, msg.data(), nullptr, 0);
}

void Window::Resize(RectInt const& rect)
{
    m_Rect = rect;
    SetWindowPos(m_WindowHandle, nullptr, m_Rect.x, m_Rect.y, m_Rect.width, m_Rect.height, 0);
}

void Window::Minimize()
{
    ShowWindow(m_WindowHandle, SW_MINIMIZE);
}

void Window::Maximize()
{
    ShowWindow(m_WindowHandle, SW_MAXIMIZE);
}

void Window::Show()
{
    if (m_IsShowing)
    {
        return;
    }

    ShowWindow(m_WindowHandle, SW_SHOWDEFAULT);
    m_IsShowing = true;
}

void Window::Hide()
{
    if (!m_IsShowing)
    {
        return;
    }

    ShowWindow(m_WindowHandle, SW_HIDE);
    m_IsShowing = false;
}

} // namespace mini::windows