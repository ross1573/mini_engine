#pragma once

#include "appkit.h"
#include "cocoa_application.h"
#include "cocoa_window.h"

namespace mini::cocoa {

NSApplication* CreateApplication(Application*);
NSWindow* CreateWindow(Window*);

}; // namespace mini::cocoa