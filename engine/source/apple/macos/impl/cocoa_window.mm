#import <QuartzCore/CAMetalLayer.h>

#import "cocoa_delegate.h"

@interface CocoaWindow : NSWindow
@end

@implementation CocoaWindow

- (BOOL)canBecomeKeyWindow
{
    return YES;
}

- (BOOL)canBecomeMainWindow
{
    return YES;
}

@end

@implementation CocoaWindowDelegate {
    mini::cocoa::Window* delegate;
}

- (instancetype)initWithDelegate:(mini::cocoa::Window*)windowDelegate
{
    self = [super init];
    if (self != nullptr) {
        delegate = windowDelegate;
    }

    return self;
}

- (BOOL)windowShouldClose:(NSWindow*)window
{
    return delegate->shouldClose();
}

- (void)windowWillClose:(NSNotification*)notification
{
    delegate->willClose();
}

- (NSSize)windowWillResize:(NSWindow*)sender toSize:(NSSize)frameSize
{
    return frameSize;
}

@end

namespace mini::cocoa {

Window::Window(mini::cocoa::Application* application)
    : m_window(nullptr)
    , m_view(nullptr)
{
    auto x = options::x;
    auto y = options::y;
    auto width = options::width;
    auto height = options::height;
    auto fullScreen = options::fullscreen;

    NSWindowStyleMask mask = NSWindowStyleMaskClosable | NSWindowStyleMaskTitled | NSWindowStyleMaskMiniaturizable |
                             NSWindowStyleMaskResizable;

    NSScreen* screen = [NSScreen mainScreen];
    NSRect windowRect = NSMakeRect(x, y, width, height);

    windowRect.origin.x = (screen.frame.size.width / 2) - (windowRect.size.width / 2);
    windowRect.origin.y = (screen.frame.size.height / 2) - (windowRect.size.height / 2);

    CocoaWindowDelegate* windowDelegate = [[CocoaWindowDelegate alloc] initWithDelegate:this];
    m_view = [[CocoaView alloc] initWithDelegate:application frameRect:windowRect];
    m_window = [[CocoaWindow alloc] initWithContentRect:windowRect
                                              styleMask:mask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO
                                                 screen:screen];

    m_window.releasedWhenClosed = NO;
    m_window.minSize = NSMakeSize(640, 360);
    m_window.delegate = windowDelegate;
    m_window.contentView = m_view;

    [m_window makeFirstResponder:m_view];
    [windowDelegate release];
    setFullScreen(fullScreen);
}

Window::~Window()
{
    m_window = nullptr;
    m_view = nullptr;
}

void Window::minimize()
{
    if (minimized()) {
        [m_window deminiaturize:nil];
    } else {
        [m_window performMiniaturize:nil];
    }
}

void Window::maximize()
{
    [m_window zoom:nil];
}

void Window::show()
{
    [m_window makeKeyAndOrderFront:nil];
}

void Window::hide()
{
    [m_window orderOut:nil];
}

bool Window::minimized() const
{
    return m_window.miniaturized;
}

bool Window::maximized() const
{
    return m_window.zoomed;
}

bool Window::fullScreen() const
{
    return (m_window.styleMask & NSWindowStyleMaskFullScreen) != 0u;
}

void Window::setFullScreen(bool active)
{
    if (fullScreen() == active) {
        return;
    }

    [m_window toggleFullScreen:m_window];
}

void Window::setMetalLayer(CA::MetalLayer* layer)
{
    m_view.wantsLayer = YES;
    m_view.layer = (__bridge CAMetalLayer*)layer;
}

} // namespace mini::cocoa