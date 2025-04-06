#import "cocoa_window_delegate.h"
#include "option.h"

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

@implementation CocoaView

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)acceptsFirstMouse:(NSEvent*)event
{
    return YES;
}

- (void)keyDown:(NSEvent*)event
{
    [super keyDown:event];
}

- (void)keyUp:(NSEvent*)event
{
    [super keyUp:event];
}

- (void)flagsChanged:(NSEvent*)event
{
    [super flagsChanged:event];
}

- (void)mouseDown:(NSEvent*)event
{
    [super mouseDown:event];
}

- (void)rightMouseDown:(NSEvent*)event
{
    [super rightMouseDown:event];
}

- (void)otherMouseDown:(NSEvent*)event
{
    [super otherMouseDown:event];
}

- (void)mouseUp:(NSEvent*)event
{
    [super mouseUp:event];
}

- (void)rightMouseUp:(NSEvent*)event
{
    [super rightMouseUp:event];
}

- (void)otherMouseUp:(NSEvent*)event
{
    [super otherMouseUp:event];
}

- (void)mouseMoved:(NSEvent*)event
{
    [super mouseMoved:event];
}

- (void)mouseDragged:(NSEvent*)event
{
    [self mouseMoved:event];
}

- (void)rightMouseDragged:(NSEvent*)event
{
    [self mouseMoved:event];
}

- (void)otherMouseDragged:(NSEvent*)event
{
    [self mouseMoved:event];
}

- (void)mouseEntered:(NSEvent*)event
{
    [super mouseEntered:event];
}

- (void)mouseExited:(NSEvent*)event
{
    [super mouseExited:event];
}

@end

namespace mini::cocoa {

Window::Window()
    : m_Window(nullptr)
    , m_View(nullptr)
{
    auto x = options::x;
    auto y = options::y;
    auto width = options::width;
    auto height = options::height;

    NSWindowStyleMask mask = NSWindowStyleMaskClosable | NSWindowStyleMaskTitled |
                             NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

    NSScreen* screen = [NSScreen mainScreen];
    NSRect windowRect = NSMakeRect(x, y, width, height);

    windowRect.origin.x = (screen.frame.size.width / 2) - (windowRect.size.width / 2);
    windowRect.origin.y = (screen.frame.size.height / 2) - (windowRect.size.height / 2);

    m_View = [[CocoaView alloc] init];
    m_Window = [[NSWindow alloc] initWithContentRect:windowRect
                                           styleMask:mask
                                             backing:NSBackingStoreBuffered
                                               defer:NO
                                              screen:screen];

    m_Window.releasedWhenClosed = NO;
    m_Window.minSize = NSMakeSize(640, 360);
    m_Window.delegate = [[CocoaWindowDelegate alloc] initWithDelegate:this];
    m_Window.contentView = m_View;
}

Window::~Window()
{
    [m_View release];
    [m_Window release];
}

void Window::Show()
{
    [m_Window orderFront:nil];
}

void Window::Hide()
{
    [m_Window orderOut:nil];
}

void Window::SetMetalLayer(CAMetalLayer* layer)
{
    m_View.wantsLayer = YES;
    m_View.layer = layer;
}

} // namespace mini::cocoa