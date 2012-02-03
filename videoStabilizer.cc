#include "videoStabilizer.h"
#include <QDebug>
#include <iostream>

videoStabilizer::videoStabilizer(QRect videoSize,
                                 QObject *parent):
    QObject(parent),
    currentGrayCodeIndex(0)
{
    //TODO: Validate height and width > 0 and less than a sensible number
    videoHeight = videoSize.height();
    videoWidth  = videoSize.width();

    grayCodeMatrix[0].resize(videoHeight);
    grayCodeMatrix[1].resize(videoHeight);
    imageMatrix.resize(videoHeight);

    for (int ii = 0; ii < videoHeight; ii++){
        grayCodeMatrix[0][ii] =new QBitArray(videoWidth);
        grayCodeMatrix[1][ii] =new QBitArray(videoWidth);
        imageMatrix[ii] = new uchar[videoWidth];
    }
}

videoStabilizer::~videoStabilizer(){
    for (int ii = 0; ii < videoHeight; ii++){
        delete grayCodeMatrix[0][ii];
        delete grayCodeMatrix[1][ii];
        delete imageMatrix[ii];
    }
}


void videoStabilizer::stabilizeImage(QImage* imageSrc, QImage* imageDest){
    convertImageToMatrix(imageSrc);
    getGrayCode();


    populateImageResult(imageDest);
    currentGrayCodeIndex^=1;
}


void videoStabilizer::convertImageToMatrix(QImage* imageSrc){
    for (int ii=0; ii < videoHeight; ii++){
        imageMatrix[ii] = imageSrc->scanLine(ii);
    }
}

void videoStabilizer::getGrayCode(){
    for (int ii = 0; ii<this->videoHeight; ii++ ){
        for (int jj = 0; jj < this->videoWidth; jj++){
            (*grayCodeMatrix[currentGrayCodeIndex][ii]).setBit(jj, getByteGrayCode(imageMatrix[ii][jj]));
        }
    }
}


inline bool videoStabilizer::getByteGrayCode (uchar value){
    return (bool) ((value & GC_FIRST_BITPLANE) ^ (value & GC_SECOND_BITPLANE));
}

void videoStabilizer::populateImageResult(QImage* imageDest){
    for (int ii = 0; ii<this->videoHeight; ii++ ){
        for (int jj = 0; jj < this->videoWidth; jj++){
            imageDest->setPixel(jj,ii,(uchar)((*grayCodeMatrix[currentGrayCodeIndex][ii]).testBit(jj))*255);
        }
    }
}

void computeSubframeCorrelation (QVector<QBitArray*> g_k,
                                 QVector<QBitArray*> g_k_m1,
                                 QVector<QByteArray*> c_j){

}
