module;

#include "cocoa/application.h"

module mini.macos;

import mini.core;

namespace mini::macos {

Window::Window()
    : m_Window(nullptr)
{
}

Window::~Window()
{
}

bool Window::Initialize()
{
    m_Window = CreateWindow(static_cast<WindowDelegate*>(this));
    return true;
}

void Window::DialogCritical(String const&)
{
}

void Window::Resize(RectInt const&)
{
}

void Window::Minimize()
{
}

void Window::Maximize()
{
}

void Window::Show()
{
}

void Window::Hide()
{
}

} // namespace mini::macos