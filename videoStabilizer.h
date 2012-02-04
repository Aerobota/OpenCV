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
#include <limits>
/**
@def    This defines determine the  the two bitplanes to be used in the
        Gray code calculation.
*/
#define GC_FIRST_BITPLANE       32
#define GC_SECOND_BITPLANE      64

#define SEARCH_FACTOR_P         8
#define HORIZ_WINDOW_M          32
#define VERT_WINDOW_N           32

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

    typedef QVector<QBitArray>  tGrayCodeMat;

    typedef QVector<uchar*> tImageMat;
    typedef QVector<uint*> tCorrelationMat;

    typedef struct _tLocalMinima{
        int     m;
        int     n;
        uint      value;
    }tLocalMinima;

    typedef struct _tSearchWindow{
        uint    lx;
        uint    ly;
        uint    rx;
        uint    ry;
    } tSearchWindow;

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
        This function computes the overall correlation of the subframes
    */
    void computeCorrelation();
    /**
        This function computes the Subframe correlation measures. It works over
    Gray coded images.

    @param  subframe    The subframe beinc computed
    @note   The correlation relies on some values #defined in the class' header
    */
    void computeSubframeCorrelation (uchar subframe, uchar t_m1);

    /**
        Compute single correlation for m,n offset;
    */
    void computeSingleCorrelation (uchar subframe, uchar t_m1, uint m, uint n);



    /** Holds the height of the video */
    int videoHeight;
    /** Holds the width of the video */
    int videoWidth;
    /** Holds the current index of the grayCodeMatrix being used*/
    uchar currentGrayCodeIndex;
    /** Holds the size of the search window based on the search factor*/
    const uchar searchFactorWindow;
    /** Holds the vertical search offset for subframes LR and LL*/
    const uint vSearchOffset;
    /** Holds the horizontal search offset for subframes UR and LR*/
    const uint hSearchOffset;

    tSearchWindow subframeLocations[4];



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

    /**
    This variable holds the correlation matrix computed of each subframe
    0 -> UL
    1 -> UR
    2 -> LL
    3 -> LR
    */
    tCorrelationMat correlationMatrix[4];

    /** This array holds the local minima of each subframe */
    tLocalMinima localMinima[4];



};

#endif // VIDEOSTABILIZER_H
