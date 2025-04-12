#import <Foundation/NSObjCRuntime.h>
#import <Foundation/NSString.h>

void DarwinLog(char const* msg)
{
    NSLog(@"%s", msg);
}