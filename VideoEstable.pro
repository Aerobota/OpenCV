#-------------------------------------------------
#
# Project created by QtCreator 2011-05-04T10:36:07
#
#-------------------------------------------------

include(vlc/VLC.pri)

QT       += core gui opengl network phonon multimedia

TARGET = VideoEstable
TEMPLATE = app

ICON = icons/macx.icns

QTFFMPEGWRAPPER_SOURCE_PATH = QTFFmpegWrapper

FFMPEG_LIBRARY_PATH = /usr/local/Cellar/ffmpeg/0.10/lib

FFMPEG_INCLUDE_PATH = QTFFmpegWrapper

SOURCES += main.cpp\
    DataMatrix.cpp \
    videoStabilizer.cc \
    PhononPlayer.cpp \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.cpp \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.cpp \
    FfmpegPlayer.cpp


HEADERS  += \
    DataMatrix.h \
    videoStabilizer.h \
    PhononPlayer.h \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.h \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.h \
    FfmpegPlayer.h

RESOURCES += \
    resources.qrc

LIBS += -lavutil \
    -lavcodec \
    -lavformat \
    -lswscale

LIBS += -L$$FFMPEG_LIBRARY_PATH
INCLUDEPATH += QVideoEncoder
INCLUDEPATH += $$FFMPEG_INCLUDE_PATH

FORMS += \
    FfmpegPlayer.ui
