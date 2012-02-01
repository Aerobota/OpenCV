/**
 * @file     videoStabilizer.h
 * @brief    This class implements a video stabilization
 * @author   Mariano I. Lizarraga

  */

#ifndef VIDEOSTABILIZER_H
#define VIDEOSTABILIZER_H

#include <QObject>
#include <QImage>


class videoStabilizer : public QObject
{
    Q_OBJECT
public:
    /**
      This is the class constructor
    @param  videoSize           A Rect of the first frame in the video
    @return firstGrayCodeBit    The first in the two bits used for the gray code computation
    */
    explicit videoStabilizer(QRect videoSize, int firstGrayCodeBit, QObject *parent = 0);

signals:
    
public slots:
    void stabilizeImage(QImage* imageSrc, QImage* imageDest);

private:

    /**
      This function converts an RGB image into a ByteArray B&W Image
    @param  imageSrc  The full image to convert
    @return imageBW   The B&W Byte Array
    */
    void toBW(QImage* imageSrc, QByteArray* imageBW);


    /**
      This function computes the graycode of an image
    @param  imageSrc        The full image for which the gray code will be computed
    @return imageGrayCode   The gray coded image
    */
    void getGrayCode(QImage* imageSrc, QByteArray* imageGrayCode);


    int videoHeight;
    int videoWidth;
    int firstGrayCodeBit;
};

#endif // VIDEOSTABILIZER_H
