#import <QuartzCore/QuartzCore.h>
#include <Cocoa/Cocoa.h>
#include <AppKit/AppKit.h>


@interface VLCVideoView : NSView
{
    id delegate;
    NSColor * backColor;
    BOOL stretchesVideo;
    id layoutManager;
    BOOL hasVideo;
}

/* Properties */
@property (assign) id delegate;
@property (copy) NSColor *backColor;
//@property BOOL fillScreen;
@property (readonly) BOOL hasVideo;
@end
