#include <QtGui/QApplication>
#include "VLCVideoWidget.h"
#include "PhononPlayer.h"
#include "FfmpegPlayer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //VLCVideoWidget w("/", 0);
    //PhononPlayer w;
    FfmpegPlayer w;
    w.show();

    return a.exec();
}
