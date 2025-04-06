#import "cocoa_delegate.h"
#include "option.h"

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

namespace mini::cocoa {

Application::Application()
    : m_AutoReleasePool(nullptr)
    , m_Application(nullptr)
{
    m_Application = [NSApplication sharedApplication];
    m_AutoReleasePool = [[NSAutoreleasePool alloc] init];

    m_Application.mainMenu = [[NSMenu alloc] init];
    [m_Application.mainMenu addItem:CreateAboutMenu()];
    [m_Application.mainMenu addItem:CreateWindowMenu()];

    m_Application.delegate = [[CocoaApplicationDelegate alloc] initWithDelegate:this];
}

Application::~Application()
{
    [m_Application release];
    [m_AutoReleasePool release];
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