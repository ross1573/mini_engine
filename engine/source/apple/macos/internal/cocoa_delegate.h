#ifndef COCOA_DELEGATE_H
#define COCOA_DELEGATE_H

#include "appkit.h"
#include "cocoa_application.h"
#include "cocoa_window.h"

@interface CocoaApplicationDelegate : NSObject <NSApplicationDelegate>

- (instancetype)initWithDelegate:(mini::cocoa::Application*)delegate;

@end

@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

- (instancetype)initWithDelegate:(mini::cocoa::Window*)delegate;

@end

@interface CocoaView : NSView

- (instancetype)initWithDelegate:(mini::cocoa::Application*)delegate frameRect:(NSRect)frameRect;

@end

#endif // COCOA_DELEGATE_H