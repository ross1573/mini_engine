#pragma once

#import <AppKit/AppKit.h>

#include "cocoa_window.h"

@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

- (instancetype)initWithDelegate:(mini::cocoa::Window*)delegate;

@end