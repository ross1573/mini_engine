#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

#include "application_delegate.h"
#include "option.h"
#include "window_delegate.h"

namespace mini::macos {

NSMenuItem* CreateAboutMenu()
{
    auto className = options::name;

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

    NSMenuItem* hide_other_item = [appMenu addItemWithTitle:@"Hide Others"
                                                     action:@selector(hideOtherApplications:)
                                              keyEquivalent:@"h"];
    hide_other_item.keyEquivalentModifierMask = NSEventModifierFlagOption |
                                                NSEventModifierFlagCommand;

    [appMenu addItemWithTitle:@"Show All"
                       action:@selector(unhideAllApplications:)
                keyEquivalent:@""];

    [appMenu addItem:[NSMenuItem separatorItem]];

    [appMenu addItemWithTitle:[@"Quit " stringByAppendingString:classStr]
                       action:@selector(terminate:)
                keyEquivalent:@"q"];

    [appMenu addItem:[NSMenuItem separatorItem]];

    appMenuItem.submenu = appMenu;
    return appMenuItem;
}

NSMenuItem* CreateWindowMenu()
{
    NSMenu* windowsMenu = [[NSMenu alloc] initWithTitle:@"Window"];
    NSMenuItem* windowsMenuItem = [[NSMenuItem alloc] initWithTitle:@""
                                                             action:nil
                                                      keyEquivalent:@""];

    [windowsMenu addItemWithTitle:NSLocalizedString(@"Minimize", @"")
                           action:@selector(performMiniaturize:)
                    keyEquivalent:@"m"];

    windowsMenuItem.submenu = windowsMenu;
    return windowsMenuItem;
}

NSApplication* CreateApplication(ApplicationDelegate* delegate)
{
    NSApplication* application = [NSApplication sharedApplication];

    application.mainMenu = [[NSMenu alloc] init];
    [application.mainMenu addItem:CreateAboutMenu()];
    [application.mainMenu addItem:CreateWindowMenu()];

    application.delegate = [[CocoaApplicationDelegate alloc] initWithDelegate:delegate];

    return application;
}

NSWindow* CreateWindow(WindowDelegate* windowDelegate)
{
    auto x = options::x;
    auto y = options::y;
    auto width = options::width;
    auto height = options::height;

    NSWindowStyleMask mask = NSWindowStyleMaskClosable | NSWindowStyleMaskTitled |
                             NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

    NSScreen* screen = [NSScreen mainScreen];
    NSRect windowRect = NSMakeRect(x, y, width, height);

    windowRect.origin.x = (screen.frame.size.width / 2) - (windowRect.size.width / 2);
    windowRect.origin.y = (screen.frame.size.height / 2) - (windowRect.size.height / 2);

    NSWindow* window = [[NSWindow alloc] initWithContentRect:windowRect
                                                   styleMask:mask
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO
                                                      screen:screen];

    window.releasedWhenClosed = NO;
    window.minSize = NSMakeSize(640, 360);
    window.delegate = [[CocoaWindowDelegate alloc] initWithDelegate:windowDelegate];

    return window;
}

} // namespace mini::macos