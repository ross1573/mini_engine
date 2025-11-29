#ifndef APPKIT_H
#define APPKIT_H

#ifdef __OBJC__
#  import <AppKit/AppKit.h>
#else

class NSApplication;
class NSWindow;
class NSView;

#endif // __OBJC__

#endif // APPKIT_H