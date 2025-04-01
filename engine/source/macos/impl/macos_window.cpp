module;

#include "cocoa.h"

module mini.macos;

import mini.core;

namespace mini::macos {

Window::Window()
{
}

Window::~Window()
{
}

bool Window::Initialize()
{
    m_Window = cocoa::CreateWindow(static_cast<cocoa::Window*>(this));
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

} // namespace mini::macos