#ifndef VLCMACWIDGET_H
#define VLCMACWIDGET_H
#include <QtGui>
#import <qmaccocoaviewcontainer_mac.h>
#include "vlc/vlc.h"


class VLCMacWidget : public QMacCocoaViewContainer
{
public:
    VLCMacWidget(libvlc_media_player_t *mediaPlayer, QWidget *parent=0);

};

#endif // VLCMACWIDGET_H
