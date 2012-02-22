#-------------------------------------------------
#
# Project created by QtCreator 2012-02-18T11:48:57
#
#-------------------------------------------------

QT       += core gui

TARGET = openCVStabilizer
TEMPLATE = app

#*****************************
INCLUDEPATH += "/opt/local/include/opencv2/"    #Include this for OpenCV 2.2.
#*****************************

#*****************************
#Include these for OpenCV 2.2.
#*****************************
LIBS += "/opt/local/lib/libopencv_calib3d.dylib"
LIBS += "/opt/local/lib/libopencv_contrib.dylib"
LIBS += "/opt/local/lib/libopencv_core.dylib"
LIBS += "/opt/local/lib/libopencv_features2d.dylib"
LIBS += "/opt/local/lib/libopencv_flann.dylib"
LIBS += "/opt/local/lib/libopencv_gpu.dylib"
LIBS += "/opt/local/lib/libopencv_highgui.dylib"
LIBS += "/opt/local/lib/libopencv_imgproc.dylib"
LIBS += "/opt/local/lib/libopencv_legacy.dylib"
LIBS += "/opt/local/lib/libopencv_ml.dylib"
LIBS += "/opt/local/lib/libopencv_objdetect.dylib"
LIBS += "/opt/local/lib/libopencv_video.dylib"
#*****************************

SOURCES += main.cc\
        MainWindow.cc \
    ../videoStabilizer.cc

HEADERS  += MainWindow.h \
    ../videoStabilizer.h

FORMS    += MainWindow.ui
