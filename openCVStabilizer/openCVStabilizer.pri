#*****************************
INCLUDEPATH += "/opt/local/include/opencv2/"
#*****************************

#*****************************
#Include these for OpenCV 2.2.
#*****************************
LIBS += "/opt/local/lib/libopencv_core.dylib"
LIBS += "/opt/local/lib/libopencv_highgui.dylib"
LIBS += "/opt/local/lib/libopencv_imgproc.dylib"
#*****************************

SOURCES += main.cc\
        MainWindow.cc \
        ../videoStabilizer.cc \
        OpenCVWidget.cpp \
        OverlayData.cpp

HEADERS  += MainWindow.h \
        ../videoStabilizer.h  \
        OpenCVWidget.h  \
        OverlayData.h

FORMS += MainWindow.ui  \
        OpenCVWidget.ui

RESOURCES += \
    resources.qrc
