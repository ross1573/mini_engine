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
    [NSApp stop:nil];
}