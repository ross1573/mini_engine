#include "cocoa_window.h"

#import <AppKit/AppKit.h>

#include "cocoa_window_delegate.h"

@implementation CocoaWindowDelegate {
    mini::cocoa::Window* m_Delegate;
}

- (instancetype)initWithDelegate:(mini::cocoa::Window*)delegate
{
    if (self = [super init]) {
        m_Delegate = delegate;
    }
    return self;
}

@end

namespace mini::cocoa {

void Window::Show(void* windowPtr)
{
    NSWindow* window = (NSWindow*)windowPtr;
    [window orderFront:nil];
}

void Window::Hide(void* windowPtr)
{
    NSWindow* window = (NSWindow*)windowPtr;
    [window orderOut:nil];
}

} // namespace mini::cocoa