#import "cocoa_delegate.h"

@implementation CocoaView {
    mini::cocoa::Application* m_Delegate;
}

- (instancetype)initWithDelegate:(mini::cocoa::Application*)delegate frameRect:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect]) {
        m_Delegate = delegate;
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
    if ([event keyCode] == 0x0C) {
        m_Delegate->Quit();
    }
    else if ([event keyCode] == 0x35) {
        m_Delegate->Abort();
    }
    else {
        [super keyDown:event];
    }

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