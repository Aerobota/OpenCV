#include "videoStabilizer.h"

videoStabilizer::videoStabilizer(QRect videoSize,
                                 int firstGrayCodeBit,
                                 QObject *parent):
    QObject(parent)
{
    videoHeight = videoSize.height();
    videoWidth  = videoSize.width();

    this->firstGrayCodeBit = firstGrayCodeBit;
}

void videoStabilizer::toBW (QImage* imageSrc, QByteArray* imageBW){

}


void videoStabilizer::stabilizeImage(QImage* imageSrc, QImage* imageDest){

}
