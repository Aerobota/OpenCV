#include "videoStabilizer.h"
#include <QDebug>
#include <iostream>

videoStabilizer::videoStabilizer(QRect videoSize,
                                 QObject *parent):
    QObject(parent)
{
    //TODO: Validate height and width > 0 and less than a sensible number
    videoHeight = videoSize.height();
    videoWidth  = videoSize.width();

    grayCodeMatrix.resize(videoHeight);
    imageMatrix.resize(videoHeight);

    for (int ii = 0; ii < videoHeight; ii++){
        grayCodeMatrix[ii] =new QBitArray(videoWidth);
        imageMatrix[ii] = new uchar[videoWidth];
    }

}

videoStabilizer::~videoStabilizer(){
    for (int ii = 0; ii < videoHeight; ii++){
        delete grayCodeMatrix[ii];
        delete imageMatrix[ii];
    }
}


void videoStabilizer::getGrayCode(){
    for (int ii = 0; ii<this->videoHeight; ii++ ){
        for (int jj = 0; jj < this->videoWidth; jj++){
            (*grayCodeMatrix[ii]).setBit(jj, getByteGrayCode(imageMatrix[ii][jj]));
        }
    }
}

void videoStabilizer::stabilizeImage(QImage* imageSrc, QImage* imageDest){
    convertImageToMatrix(imageSrc);
    getGrayCode();


    populateImageResult(imageDest);
}

void videoStabilizer::convertImageToMatrix(QImage* imageSrc){
    for (int ii=0; ii < videoHeight; ii++){
        imageMatrix[ii] = imageSrc->scanLine(ii);
        for (int jj = 0; jj < videoWidth; jj++)
            printf("%03d ",imageMatrix[ii][jj]);
        std::cout << endl;
    }
}

inline bool videoStabilizer::getByteGrayCode (uchar value){
    return (bool) ((value & GC_FIRST_BITPLANE) ^ (value & GC_SECOND_BITPLANE));
}

void videoStabilizer::populateImageResult(QImage* imageDest){
    for (int ii = 0; ii<this->videoHeight; ii++ ){
        for (int jj = 0; jj < this->videoWidth; jj++){
            imageDest->setPixel(ii,jj,(uchar)((*grayCodeMatrix[ii]).testBit(jj)));
        }
    }
}
