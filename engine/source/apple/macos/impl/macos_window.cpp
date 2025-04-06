module;

#include "cocoa_application.h"
#include "cocoa_window.h"

module mini.macos;

import mini.core;
import mini.engine;
import :handle;

namespace mini::macos {

Window::Window(cocoa::Application* application)
    : cocoa::Window(application)
{
}

bool Window::Initialize()
{
    return true;
}

void Window::DialogCritical(String const& msg)
{
    cocoa::Window::AlertError(msg.c_str());
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

bool Window::ShouldClose()
{
    return true;
}

void Window::WillClose()
{
    Engine::Quit();
}

} // namespace mini::macos