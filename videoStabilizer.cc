#include "videoStabilizer.h"
#include <QDebug>
#include <iostream>

videoStabilizer::videoStabilizer(QRect videoSize,
                                 QObject *parent):
    QObject(parent),
    currentGrayCodeIndex(0),
    searchFactorWindow(2*SEARCH_FACTOR_P+1),
    vSearchOffset(videoSize.height()/2),
    hSearchOffset(videoSize.width()/2)
{
    //TODO: Validate height and width > 0 and less than a sensible number
    videoHeight = videoSize.height();
    videoWidth  = videoSize.width();

    /// allocate the memory of all the Matrices
    for (int jj = 0; jj < 4; jj++) {
        correlationMatrix[jj].resize(searchFactorWindow);
        for (int ii = 0; ii < searchFactorWindow; ii++) {
            correlationMatrix[jj][ii] = new uint[searchFactorWindow];
        }
    }

    grayCodeMatrix[0].resize(videoHeight);
    grayCodeMatrix[1].resize(videoHeight);
    imageMatrix.resize(videoHeight);

    for (int ii = 0; ii < videoHeight; ii++){
        grayCodeMatrix[0][ii].resize(videoWidth);
        grayCodeMatrix[1][ii].resize(videoWidth);
        imageMatrix[ii] = new uchar[videoWidth];
    }

    /// Compute the search windows
    // UL
    subframeLocations[0].lx = hSearchOffset/2 - HORIZ_WINDOW_M/2;
    subframeLocations[0].rx = hSearchOffset/2 + HORIZ_WINDOW_M/2;
    subframeLocations[0].ly = vSearchOffset/2 - VERT_WINDOW_N/2;
    subframeLocations[0].ry = vSearchOffset/2 + VERT_WINDOW_N/2;
    // UR
    subframeLocations[1].lx = hSearchOffset/2 - HORIZ_WINDOW_M/2 + hSearchOffset;
    subframeLocations[1].rx = hSearchOffset/2 + HORIZ_WINDOW_M/2 + hSearchOffset;
    subframeLocations[1].ly = vSearchOffset/2 - VERT_WINDOW_N/2;
    subframeLocations[1].ry = vSearchOffset/2 + VERT_WINDOW_N/2;
    // LL
    subframeLocations[2].lx = hSearchOffset/2 - HORIZ_WINDOW_M/2;
    subframeLocations[2].rx = hSearchOffset/2 + HORIZ_WINDOW_M/2;
    subframeLocations[2].ly = vSearchOffset/2 - VERT_WINDOW_N/2 + vSearchOffset;
    subframeLocations[2].ry = vSearchOffset/2 + VERT_WINDOW_N/2 + vSearchOffset;
    // LR
    subframeLocations[3].lx = hSearchOffset/2 - HORIZ_WINDOW_M/2 + hSearchOffset;
    subframeLocations[3].rx = hSearchOffset/2 + HORIZ_WINDOW_M/2 + hSearchOffset;
    subframeLocations[3].ly = vSearchOffset/2 - VERT_WINDOW_N/2 + vSearchOffset;
    subframeLocations[3].ry = vSearchOffset/2 + VERT_WINDOW_N/2 + vSearchOffset;

}

videoStabilizer::~videoStabilizer(){
    for (int ii = 0; ii < videoHeight; ii++){
        delete imageMatrix[ii];
    }

    for (int jj = 0; jj < 4; jj++) {
        for (int ii = 0; ii < searchFactorWindow; ii++) {
            delete correlationMatrix[jj][ii];
        }
    }
}


void videoStabilizer::stabilizeImage(QImage* imageSrc, QImage* imageDest){
    convertImageToMatrix(imageSrc);
    getGrayCode();
    computeCorrelation();


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
            grayCodeMatrix[currentGrayCodeIndex][ii].setBit(jj, getByteGrayCode(imageMatrix[ii][jj]));
        }
    }
}


inline bool videoStabilizer::getByteGrayCode (uchar value){
    return (bool) ((value & GC_FIRST_BITPLANE) ^ (value & GC_SECOND_BITPLANE));
}

void videoStabilizer::populateImageResult(QImage* imageDest){
    for (int ii = 0; ii<this->videoHeight; ii++ ){
        for (int jj = 0; jj < this->videoWidth; jj++){
            imageDest->setPixel(jj,ii,(uchar)(grayCodeMatrix[currentGrayCodeIndex][ii].testBit(jj))*255);
        }
    }
}

void videoStabilizer::computeCorrelation(){

    uchar t_m1 = currentGrayCodeIndex ^ 1;
    memset(localMinima,0,4*sizeof(tLocalMinima));

    for (uchar ii = 0; ii < 4; ii++) {
        computeSubframeCorrelation(ii, t_m1);
    }
}

void videoStabilizer::computeSubframeCorrelation (uchar subframe, uchar t_m1){
    /**
S_tm1 = u(:,(size(u,2)/2)+1:end);
Cj = zeros(2*p+1,2*p+1);
for m = 1:2*p+1
    for n = 1:2*p+1
        cj_mat = bitxor(S_t(p+1:p+N,p+1:p+N),...
                         S_tm1(m:m+N-1,n:n+N-1));
        Cj(m,n) = sum(cj_mat(:));
    end
end
*/
    localMinima[subframe].value = UINT_MAX;

    for (uint m = 0; m < searchFactorWindow; m++) {
        for (uint n = 0; n < searchFactorWindow; n++) {

            computeSingleCorrelation(subframe, t_m1,m,n);

            // find the minimimum
            if (localMinima[subframe].value > correlationMatrix[subframe][m][n]){

                localMinima[subframe].value = correlationMatrix[subframe][m][n];
                localMinima[subframe].m = m - SEARCH_FACTOR_P;
                localMinima[subframe].n = n - SEARCH_FACTOR_P;
            } // if localMinima

        } // for n

    } // for m
}


void videoStabilizer::computeSingleCorrelation (uchar subframe, uchar t_m1, uint m, uint n){
    int m_offset = m-SEARCH_FACTOR_P, n_offset = n - SEARCH_FACTOR_P;

    for (int x = subframeLocations[subframe].lx ; x < subframeLocations[subframe].rx; x++) {     // x is width
        for (int y = subframeLocations[subframe].ly; y < subframeLocations[subframe].ry; y++) {  // y is height
            if (x == 240) {
                m_offset = m_offset - 1;
            }
            correlationMatrix[subframe][m][n] += grayCodeMatrix[currentGrayCodeIndex][x].testBit(y) ^
                                                 grayCodeMatrix[t_m1][x+m_offset].testBit(y+n_offset);
        }
    }
    m_offset = 1;
}
