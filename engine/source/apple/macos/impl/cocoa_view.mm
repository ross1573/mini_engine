#import "cocoa_delegate.h"

@implementation CocoaView {
    mini::cocoa::Application* m_delegate;
}

- (instancetype)initWithDelegate:(mini::cocoa::Application*)delegate frameRect:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect]) {
        m_delegate = delegate;
    }

    return self;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)acceptsFirstMouse:(NSEvent*)event
{
    return YES;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (void)keyDown:(NSEvent*)event
{
    m_delegate->OnKeyDown([event keyCode]);
    [self interpretKeyEvents:[NSArray arrayWithObject:event]];
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