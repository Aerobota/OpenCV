/**
 * @file     videoStabilizer.h
 * @brief    This class implements a video stabilization
 * @author   Mariano I. Lizarraga

  */

#ifndef VIDEOSTABILIZER_H
#define VIDEOSTABILIZER_H

#include <QObject>
#include <QImage>
#include <QBitArray>

/**
@def    This defines determine the  the two bitplanes to be used in the
        Gray code calculation.
*/
#define GC_FIRST_BITPLANE       32
#define GC_SECOND_BITPLANE      64


class videoStabilizer : public QObject
{
    Q_OBJECT
public:
    /**
      This is the class constructor
    @param  videoSize           A Rect of the first frame in the video
    @return firstGrayCodeBit    The first in the two bits used for the gray code computation
    */
    videoStabilizer(QRect videoSize, QObject *parent = 0);

    ~videoStabilizer( );

signals:
    
public slots:
    void stabilizeImage(QImage* imageSrc, QImage* imageDest);

private:

    /**
      This function computes the graycode of an image
    @param  imageSrc        The full image for which the gray code will be computed
    @return imageGrayCode   The gray coded image
    */
    void getGrayCode();

    /**
      This function takes a QImage and scans line by line into uchar* to imageMatrix;

    @param  imageSrc        The image to be converted.
    @note   This function populates the imageMatrix data member.
    */
    void convertImageToMatrix(QImage* imageSrc);

    /**
        This function computes the Gray Code for a single byte

    @param  value       The 8bit value to be used in the Gray code calculation. The
                        bitplanes used are #defined in the header file.
    */
    inline bool getByteGrayCode(uchar value);

    /**
        This function creates the de-rotated image to paint.

    @param  imageDest   The QImage where the final result will be painted on.
    */
    void populateImageResult(QImage* imageDest);


    /** Holds the height of the video */
    int videoHeight;
    /** Holds the width of the video */
    int videoWidth;


    /**
    This variable holds videoHeight array of QBitArrays that are
    videoWidth long.

    @see getGrayCode()
    */
    QVector<QBitArray*> grayCodeMatrix;

    /**
    This variable holds the image matrix currently being worked on.
    */
    QVector<uchar*> imageMatrix;

};

#endif // VIDEOSTABILIZER_H
