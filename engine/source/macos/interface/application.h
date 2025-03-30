#pragma once

#include "appkit.h"

class ApplicationDelegate;
class WindowDelegate;

namespace mini::macos {

NSApplication* CreateApplication(ApplicationDelegate*);
NSWindow* CreateWindow(WindowDelegate*);

}; // namespace mini::macos