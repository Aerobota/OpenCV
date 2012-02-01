#include <QtGui/QApplication>
#include "VLCVideoWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VLCVideoWidget w("/vuelos", 0);
    w.show();

    return a.exec();
}
