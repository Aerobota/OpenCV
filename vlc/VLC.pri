VLC_SOURCE = vlc

INCLUDEPATH += $$VLC_SOURCE

HEADERS += $$VLC_SOURCE/VLCInstance.h \
    $$VLC_SOURCE/VLCVideoWidget.h


SOURCES += $$VLC_SOURCE/VLCpp.hpp \
    $$VLC_SOURCE/VLCInstance.cpp \
    $$VLC_SOURCE/VLCVideoWidget.cpp \

FORMS += $$VLC_SOURCE/VLCVideoWidget.ui

INCLUDEPATH += /Applications/VLC.app/Contents/MacOS/include \

LIBS += /Applications/VLC.app/Contents/MacOS/lib/libvlc.5.dylib \
    /Applications/VLC.app/Contents/MacOS/lib/libvlccore.dylib \

macx: {
    HEADERS += \
        $$VLC_SOURCE/VLCMacWidget.h \
        $$VLC_SOURCE/VLCNSView.h

    OBJECTIVE_SOURCES += \
        $$VLC_SOURCE/VLCMacWidget.mm \
        $$VLC_SOURCE/VLCNSView.mm

    LIBS += \
        -framework AppKit \
        -framework Cocoa

    QMAKE_POST_LINK += echo "Copying files"

    exists(/Applications/VLC.app) {
        QMAKE_POST_LINK += && cp -r /Applications/VLC.app/Contents/MacOS/include VideoEstable.app/Contents/MacOS
        QMAKE_POST_LINK += && cp -r /Applications/VLC.app/Contents/MacOS/lib VideoEstable.app/Contents/MacOS
        QMAKE_POST_LINK += && cp -r /Applications/VLC.app/Contents/MacOS/plugins VideoEstable.app/Contents/MacOS
    }
}
