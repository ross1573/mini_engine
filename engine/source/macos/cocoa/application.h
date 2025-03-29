#pragma once

class NSApplication;
class NSWindow;

class Application;
class ApplicationDelegate;
class Window;
class WindowDelegate;

namespace mini::macos {

NSApplication* CreateApplication();
NSWindow* CreateWindow(WindowDelegate*);

}; // namespace mini::macos