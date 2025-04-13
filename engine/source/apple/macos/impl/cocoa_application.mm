#import "cocoa_delegate.h"
#include "option.h"

#if DEBUG
#  include <Foundation/NSDebug.h>
#endif

@implementation CocoaApplicationDelegate {
    mini::cocoa::Application* m_Delegate;
}

- (instancetype)initWithDelegate:(mini::cocoa::Application*)delegate
{
    if (self = [super init]) {
        m_Delegate = delegate;
    }

    return self;
}

- (void)applicationWillFinishLaunching:(NSNotification*)notification
{
    m_Delegate->ApplicationWillFinishLaunching((__bridge NS::Notification*)notification);
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification
{
    m_Delegate->ApplicationDidFinishLaunching((__bridge NS::Notification*)notification);
}

@end

@implementation NSMenu (Extension)

- (NSMenuItem*)addItemWithModifier:(NSString*)string
                            action:(SEL)selector
                     keyEquivalent:(NSString*)charCode
                   keyModifierMask:(NSEventModifierFlags)modifierFlags
{
    NSMenuItem* menuItem = [self addItemWithTitle:string action:selector keyEquivalent:charCode];
    menuItem.keyEquivalentModifierMask = modifierFlags;
    return menuItem;
}

- (NSMenuItem*)addHiddenItem:(NSString*)string
                      action:(SEL)selector
               keyEquivalent:(NSString*)charCode
{
    NSMenuItem* menuItem = [self addItemWithTitle:string action:selector keyEquivalent:charCode];
    menuItem.hidden = YES;
    return menuItem;
}

- (NSMenuItem*)addHiddenItem:(NSString*)string
                      action:(SEL)selector
               keyEquivalent:(NSString*)charCode
             keyModifierMask:(NSEventModifierFlags)modifierFlags
{
    NSMenuItem* menuItem = [self addItemWithTitle:string action:selector keyEquivalent:charCode];
    menuItem.keyEquivalentModifierMask = modifierFlags;
    menuItem.hidden = YES;
    return menuItem;
}

@end

NSMenuItem* CreateAboutMenu()
{
    auto className = mini::options::name;

    NSString* classStr = [NSString stringWithUTF8String:className];
    NSMenu* appMenu = [[NSMenu alloc] initWithTitle:@""];
    NSMenuItem* appMenuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];

    [appMenu addItemWithTitle:[@"About " stringByAppendingString:classStr]
                       action:@selector(orderFrontStandardAboutPanel:)
                keyEquivalent:@""];

    [appMenu addItem:[NSMenuItem separatorItem]];

    [appMenu addItemWithTitle:[@"Hide " stringByAppendingString:classStr]
                       action:@selector(hide:)
                keyEquivalent:@"h"];

    [appMenu addItemWithModifier:@"Hide Others"
                          action:@selector(hideOtherApplications:)
                   keyEquivalent:@"h"
                 keyModifierMask:NSEventModifierFlagOption | NSEventModifierFlagCommand];

    [appMenu addItemWithTitle:@"Show All"
                       action:@selector(unhideAllApplications:)
                keyEquivalent:@""];

    [appMenu addItem:[NSMenuItem separatorItem]];

    [appMenu addItemWithTitle:[@"Quit " stringByAppendingString:classStr]
                       action:@selector(terminate:)
                keyEquivalent:@"q"];

    [appMenu addItem:[NSMenuItem separatorItem]];

    appMenuItem.submenu = appMenu;

    [appMenu release];
    return [appMenuItem autorelease];
}

NSMenuItem* CreateWindowMenu()
{
    NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
    NSMenuItem* windowMenuItem = [[NSMenuItem alloc] initWithTitle:@""
                                                            action:nil
                                                     keyEquivalent:@""];

    [windowMenu addItemWithTitle:NSLocalizedString(@"Minimize", @"")
                          action:@selector(performMiniaturize:)
                   keyEquivalent:@"m"];

    [windowMenu addItemWithTitle:NSLocalizedString(@"Maximize", @"")
                          action:@selector(zoom:)
                   keyEquivalent:@""];

    [windowMenu addItem:[NSMenuItem separatorItem]];

    [windowMenu addItemWithModifier:NSLocalizedString(@"FullScreen", @"")
                             action:@selector(toggleFullScreen:)
                      keyEquivalent:@"f"
                    keyModifierMask:NSEventModifierFlagFunction];

    [windowMenu addHiddenItem:@"FullScreen"
                       action:@selector(toggleFullScreen:)
                keyEquivalent:@"f"
              keyModifierMask:NSEventModifierFlagCommand | NSEventModifierFlagControl];

    windowMenuItem.submenu = windowMenu;

    [windowMenu release];
    return [windowMenuItem autorelease];
}

namespace mini::cocoa {

Application::Application()
    : m_AutoReleasePool(nullptr)
    , m_Application(nullptr)
{
    m_Application = [NSApplication sharedApplication];
    m_AutoReleasePool = [[NSAutoreleasePool alloc] init];

    @autoreleasepool {
        m_Application.mainMenu = [[NSMenu alloc] init];
        [m_Application.mainMenu addItem:CreateAboutMenu()];
        [m_Application.mainMenu addItem:CreateWindowMenu()];

        m_Application.delegate = [[CocoaApplicationDelegate alloc] initWithDelegate:this];
    }
}

Application::~Application()
{
    [m_Application release];
    [m_AutoReleasePool release];
#if DEBUG
    [NSAutoreleasePool showPools];
#endif
}

void Application::Launch()
{
    if (![[NSRunningApplication currentApplication] isFinishedLaunching]) {
        [m_Application finishLaunching];
    }
}

void Application::PollEvents()
{
    @autoreleasepool {
        while (true) {
            NSEvent* event = [m_Application nextEventMatchingMask:NSEventMaskAny
                                                        untilDate:[NSDate distantPast]
                                                           inMode:NSDefaultRunLoopMode
                                                          dequeue:YES];

            if (event == nil) {
                break;
            }

            [m_Application sendEvent:event];
        }
    } // autoreleasepool
}

} // namespace mini::cocoa