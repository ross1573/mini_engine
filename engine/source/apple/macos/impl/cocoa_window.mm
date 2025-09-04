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
    mini::cocoa::Window* m_Delegate;
}

- (instancetype)initWithDelegate:(mini::cocoa::Window*)delegate
{
    if (self = [super init]) {
        m_Delegate = delegate;
    }
    return self;
}

- (BOOL)windowShouldClose:(NSWindow*)window
{
    return m_Delegate->ShouldClose();
}

- (void)windowWillClose:(NSNotification*)notification
{
    m_Delegate->WillClose();
}

- (NSSize)windowWillResize:(NSWindow*)sender toSize:(NSSize)frameSize
{
    return frameSize;
}

@end

namespace mini::cocoa {

Window::Window(mini::cocoa::Application* application)
    : m_Window(nullptr)
    , m_View(nullptr)
{
    auto x = options::x;
    auto y = options::y;
    auto width = options::width;
    auto height = options::height;
    auto fullScreen = options::fullscreen;

    NSWindowStyleMask mask = NSWindowStyleMaskClosable | NSWindowStyleMaskTitled |
                             NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

    NSScreen* screen = [NSScreen mainScreen];
    NSRect windowRect = NSMakeRect(x, y, width, height);

    windowRect.origin.x = (screen.frame.size.width / 2) - (windowRect.size.width / 2);
    windowRect.origin.y = (screen.frame.size.height / 2) - (windowRect.size.height / 2);

    CocoaWindowDelegate* windowDelegate = [[CocoaWindowDelegate alloc] initWithDelegate:this];
    m_View = [[CocoaView alloc] initWithDelegate:application frameRect:windowRect];
    m_Window = [[CocoaWindow alloc] initWithContentRect:windowRect
                                              styleMask:mask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO
                                                 screen:screen];

    m_Window.releasedWhenClosed = NO;
    m_Window.minSize = NSMakeSize(640, 360);
    m_Window.delegate = windowDelegate;
    m_Window.contentView = m_View;

    [m_Window makeFirstResponder:m_View];
    [windowDelegate release];
    SetFullScreen(fullScreen);
}

Window::~Window()
{
    [m_View release];
    [m_Window release];
}

void Window::AlertError(char const* msg)
{
    @autoreleasepool {
        NSString* errMsg = [NSString stringWithUTF8String:msg];
        NSAlert* alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"OK"];
        [alert setMessageText:@"Error"];
        [alert setInformativeText:errMsg];
        [alert setAlertStyle:NSAlertStyleCritical];
        [alert runModal];
    }
}

void Window::Minimize()
{
    if (IsMinimized()) {
        [m_Window deminiaturize:nil];
    }
    else {
        [m_Window performMiniaturize:nil];
    }
}

void Window::Maximize()
{
    [m_Window zoom:nil];
}

void Window::Show()
{
    [m_Window makeKeyAndOrderFront:nil];
}

void Window::Hide()
{
    [m_Window orderOut:nil];
}

bool Window::IsMinimized() const
{
    return m_Window.miniaturized;
}

bool Window::IsMaximized() const
{
    return m_Window.zoomed;
}

bool Window::IsFullScreen() const
{
    return m_Window.styleMask & NSWindowStyleMaskFullScreen;
}

void Window::SetFullScreen(bool active)
{
    if (IsFullScreen() == active) {
        return;
    }

    [m_Window toggleFullScreen:m_Window];
}

void Window::SetMetalLayer(CA::MetalLayer* layer)
{
    m_View.wantsLayer = YES;
    m_View.layer = (__bridge CAMetalLayer*)layer;
}

} // namespace mini::cocoa