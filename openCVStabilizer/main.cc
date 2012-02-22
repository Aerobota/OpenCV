#include <QtGui/QApplication>
#include "MainWindow.h"
#include "OpenCVWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    OpenCVWidget w;
    w.show();
    
    return a.exec();
}
