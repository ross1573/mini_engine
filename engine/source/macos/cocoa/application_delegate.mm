#import "cocoa/application_delegate.h"

#import <AppKit/AppKit.h>

@implementation CocoaApplicationDelegate {
    ApplicationDelegate* m_Delegate;
}

- (instancetype)initWithDelegate:(ApplicationDelegate*)delegate
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

void ApplicationDelegate::Run()
{
    if (![[NSRunningApplication currentApplication] isFinishedLaunching]) [NSApp run];
}

void ApplicationDelegate::Stop()
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