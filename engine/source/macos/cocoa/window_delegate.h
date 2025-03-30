#pragma once

#import <AppKit/AppKit.h>

#include "../public/window_delegate.h"

@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

- (instancetype)initWithDelegate:(WindowDelegate*)delegate;

@end