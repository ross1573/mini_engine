module;

#include "cocoa_application.h"
#include "cocoa_window.h"

module mini.macos;

import mini.core;
import :handle;

namespace mini::macos {

Window::Window(cocoa::Application* application)
    : cocoa::Window(application)
{
}

void Window::DialogCritical(String const& msg)
{
    cocoa::Window::AlertError(msg.Data());
}

void Window::Resize(RectInt const&)
{
}

} // namespace mini::macos