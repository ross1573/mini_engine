#pragma once

class NSApplication;
class NSWindow;

class CocoaApplication;
class CocoaApplicationDelegate;
class CocoaWindowDelegate;

struct ApplicationDelegate;
struct WindowDelegate;

namespace mini::macos {

NSApplication* CreateApplication(ApplicationDelegate*);
NSWindow* CreateWindow(WindowDelegate*);

}; // namespace mini::macos