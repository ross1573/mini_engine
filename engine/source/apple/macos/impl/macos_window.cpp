module;

#include "cocoa_window.h"

module mini.macos;

import mini.core;

namespace mini::macos {

Window::Window()
    : cocoa::Window()
{
}

bool Window::Initialize()
{
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