#import "cocoa_window_delegate.h"

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

Window::Window()
    : m_Window(nullptr)
{
}

void Window::Show()
{
    [m_Window orderFront:nil];
}

void Window::Hide()
{
    [m_Window orderOut:nil];
}

} // namespace mini::cocoa