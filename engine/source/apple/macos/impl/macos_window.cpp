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

void Window::resize([[maybe_unused]] RectInt const& windowSize)
{
}

} // namespace mini::macos