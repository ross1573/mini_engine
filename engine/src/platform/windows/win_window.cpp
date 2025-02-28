module;

#include <windows.h>
#include "core/define.h"
#include "resource/Resource.h"
#include "platform/windows/assert.h"

module mini.windows:window;

import mini.core;
import mini.math;
import mini.container;
import mini.windows;
import mini.platform;
import mini.options;

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
    auto className = StringConverter<char, wchar_t>(mini::options::name);
    auto titleName = StringConverter<char, wchar_t>(mini::options::title);
    m_Rect = mini::options::windowRect;

    HINSTANCE instance = Platform::GetHandle<Handle>()->GetHINSTANCE();
    uint32 styleEx = WS_EX_APPWINDOW;
    uint32 style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;

    if (mini::options::resizableWindow)
    {
        style |= WS_THICKFRAME;
    }

    m_WindowHandle = CreateWindowExW(styleEx,
                                     className.Data(),
                                     titleName.Data(),
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
    wchar_t buf[256]; // make sure there's no more allocation
    SizeT len = mini::memory::ConvertLength(msg.data(), (SizeT)msg.size());
    mini::memory::Convert(msg.data(), (SizeT)msg.size(), buf, len);
    buf[255] = '\0';

    MessageBoxW(m_WindowHandle, buf, nullptr, 0);
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