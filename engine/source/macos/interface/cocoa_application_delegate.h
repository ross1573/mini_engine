#pragma once

#import <AppKit/AppKit.h>

#include "cocoa_application.h"

@interface CocoaApplicationDelegate : NSObject <NSApplicationDelegate>

- (instancetype)initWithDelegate:(mini::cocoa::Application*)delegate;

- (void)applicationWillFinishLaunching:(NSNotification*)notification;
- (void)applicationDidFinishLaunching:(NSNotification*)notification;

@end