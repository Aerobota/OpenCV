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

    typedef QVector<QBitArray*>  tGrayCodeMat;
    typedef QVector<uchar*> tImageMat;

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

    /**
        This function computes the Subframe correlation measures. It works over
    Gray coded images.
    @param  g_k     The Gray code of the current frame
    @param  g_k_m1  The Gray code of the frame at t-1
    @return C_j     The correlation matrix

    @note   The correlation relies on some values #defined in the class' header
    */
    void computeSubframeCorrelation (QVector<QBitArray*> g_k,
                                     QVector<QBitArray*> g_k_m1,
                                     QVector<QByteArray*> c_j);


    /** Holds the height of the video */
    int videoHeight;
    /** Holds the width of the video */
    int videoWidth;
    /** Holds the current index of the grayCodeMatrix being used*/
    uchar currentGrayCodeIndex;


    /**
    This variable is an array of vectors that are in turn videoHeight arrays of QBitArrays that are
    videoWidth long. They take turns to hold g_k[t] and g_k[t-1]

    @see getGrayCode()
    */
    tGrayCodeMat grayCodeMatrix[2];

    /**
    This variable holds the image matrix currently being worked on.
    */
    tImageMat imageMatrix;

};

#endif // VIDEOSTABILIZER_H
