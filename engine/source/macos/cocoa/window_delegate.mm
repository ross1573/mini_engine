#import "cocoa/window_delegate.h"

@implementation CocoaWindowDelegate {
    WindowDelegate* m_Delegate;
}

- (instancetype)initWithDelegate:(WindowDelegate*)delegate
{
    if (self = [super init]) {
        m_Delegate = delegate;
    }
    return self;
}

@end