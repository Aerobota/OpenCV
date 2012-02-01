#include "VLCMacWidget.h"
#import <Cocoa/Cocoa.h>
#include <AppKit/AppKit.h>
#include <QtDebug>
#include "VLCNSView.h"

VLCMacWidget::VLCMacWidget(libvlc_media_player_t *mediaPlayer, QWidget *parent) : QMacCocoaViewContainer(0, parent)
{
    parent->setWindowTitle("Reproductor de Video");
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    //NSView *videoView = [[NSView alloc] init];
    VLCVideoView *videoView = [[VLCVideoView alloc] init];
    [videoView setAutoresizingMask: NSViewHeightSizable|NSViewWidthSizable];
    this->setCocoaView(videoView);
    //this->resize(500, 500);
    libvlc_media_player_set_nsobject(mediaPlayer, this->cocoaView());

    [videoView release];
    [pool release];


}




@interface VLCOpenGLVoutView : NSView
//- (void)detachFromVout;
@end
