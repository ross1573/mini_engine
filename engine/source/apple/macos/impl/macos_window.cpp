module mini.macos;

import mini.core;
import :handle;

namespace mini::macos {

Window::Window(cocoa::Application* application)
    : cocoa::Window(application)
{
}

void Window::Resize(RectInt const&)
{
}

} // namespace mini::macos