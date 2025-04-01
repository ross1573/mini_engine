#include "cocoa_window.h"

#import <AppKit/AppKit.h>

#include "cocoa_window_delegate.h"

@implementation CocoaWindowDelegate {
    mini::cocoa::Window* m_Delegate;
}

- (instancetype)initWithDelegate:(mini::cocoa::Window*)delegate
{
    if (self = [super init]) {
        m_Delegate = delegate;
    }
    return self;
}

@end

namespace mini::cocoa {

}