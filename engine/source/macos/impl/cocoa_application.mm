#include "cocoa_application.h"

#import <AppKit/AppKit.h>

#include "cocoa_application_delegate.h"

@implementation CocoaApplicationDelegate {
    mini::cocoa::Application* m_Delegate;
}

- (instancetype)initWithDelegate:(mini::cocoa::Application*)delegate
{
    if (self = [super init]) {
        m_Delegate = delegate;
    }

    return self;
}

- (void)applicationWillFinishLaunching:(NSNotification*)notification
{
    m_Delegate->ApplicationWillFinishLaunching((__bridge NS::Notification*)notification);
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification
{
    m_Delegate->ApplicationDidFinishLaunching((__bridge NS::Notification*)notification);
}

@end

namespace mini::cocoa {

void Application::Run()
{
    if (![[NSRunningApplication currentApplication] isFinishedLaunching]) [NSApp run];
}

void Application::Stop()
{
    @autoreleasepool {
        NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                            location:NSMakePoint(0, 0)
                                       modifierFlags:0
                                           timestamp:0
                                        windowNumber:0
                                             context:nil
                                             subtype:0
                                               data1:0
                                               data2:0];

        [NSApp postEvent:event atStart:YES];
    }

    [NSApp stop:nil];
}

} // namespace mini::cocoa