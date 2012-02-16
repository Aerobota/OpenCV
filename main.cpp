#include <QtGui/QApplication>
#include "VLCVideoWidget.h"
#include "PhononPlayer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VLCVideoWidget w("/", 0);
    //PhononPlayer w;
    w.show();

    return a.exec();
}
