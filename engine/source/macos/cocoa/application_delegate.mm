#include "application_delegate.h"

#import <AppKit/AppKit.h>

@implementation ApplicationDelegate

- (void)applicationWillFinishLaunching:(NSNotification*)notification
{
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification
{
    [NSApp stop:nil];
}

@end