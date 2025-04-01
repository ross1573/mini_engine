#pragma once

#include "appkit.h"
#include "cocoa_window.h"

@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

- (instancetype)initWithDelegate:(mini::cocoa::Window*)delegate;

@end