#pragma once

#import <AppKit/AppKit.h>

#include "interface/window_delegate.h"

@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

- (instancetype)initWithDelegate:(WindowDelegate*)delegate;

@end