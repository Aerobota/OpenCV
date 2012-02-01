#-------------------------------------------------
#
# Project created by QtCreator 2011-05-04T10:36:07
#
#-------------------------------------------------

include(vlc/VLC.pri)

QT       += core gui opengl network

TARGET = VideoEstable
TEMPLATE = app

ICON = icons/macx.icns

SOURCES += main.cpp\
    DataMatrix.cpp \
    videoStabilizer.cc

HEADERS  += \
    DataMatrix.h \
    videoStabilizer.h

RESOURCES += \
    resources.qrc
