#pragma once

#import <AppKit/AppKit.h>

@interface ApplicationDelegate : NSObject <NSApplicationDelegate>

- (void)applicationWillFinishLaunching:(NSNotification*)notification;
- (void)applicationDidFinishLaunching:(NSNotification*)notification;

@end