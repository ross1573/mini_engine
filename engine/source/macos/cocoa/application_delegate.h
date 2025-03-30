#pragma once

#import <AppKit/AppKit.h>

#include "../public/application_delegate.h"

@interface CocoaApplicationDelegate : NSObject <NSApplicationDelegate>

- (instancetype)initWithDelegate:(ApplicationDelegate*)delegate;

- (void)applicationWillFinishLaunching:(NSNotification*)notification;
- (void)applicationDidFinishLaunching:(NSNotification*)notification;

@end