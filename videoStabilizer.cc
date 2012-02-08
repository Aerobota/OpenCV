#include "videoStabilizer.h"
#include <QDebug>
#include <iostream>

videoStabilizer::videoStabilizer(QRect videoSize, QObject *parent):
    QObject(parent),
    currentGrayCodeIndex(0),
    searchFactorWindow(2*SEARCH_FACTOR_P+1),
    vSearchOffset(videoSize.height()/2),
    hSearchOffset(videoSize.width()/2)
{
    //TODO: Validate height and width > 0 and less than a sensible number
    videoHeight = videoSize.height();
    videoWidth  = videoSize.width();

    allocateAndInitialize();

    computeSearchWindows();

    for (uint subframe = 0; subframe < 4; subframe++){

        computeCorrelationLocations( subframe,
                                     subframeLocations[subframe].lx + SEARCH_FACTOR_P,
                                     subframeLocations[subframe].ly + SEARCH_FACTOR_P,
                                     0,
                                    (HORIZ_WINDOW_M-2*SEARCH_FACTOR_P)/2,
                                    (VERT_WINDOW_N-2*SEARCH_FACTOR_P)/2);
    }

}

videoStabilizer::~videoStabilizer(){
    for (int ii = 0; ii < videoHeight; ii++){
        delete imageMatrix[ii];
    }

}

void videoStabilizer::allocateAndInitialize(){
    // Initialize each subframe correlation vectors
    for (uint subframe = 0; subframe < 4; subframe++) {
        memset(correlationMatrix[subframe],0, sizeof(tcorrMatElement)*27);
    }

    /// allocate the memory of all the Matrices
    grayCodeMatrix[0].resize(videoHeight);
    grayCodeMatrix[1].resize(videoHeight);
    imageMatrix.resize(videoHeight);

    for (int ii = 0; ii < videoHeight; ii++){
        grayCodeMatrix[0][ii].resize(videoWidth);
        grayCodeMatrix[1][ii].resize(videoWidth);
        imageMatrix[ii] = new uchar[videoWidth];
    }
}

void videoStabilizer::computeSearchWindows (){
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

void videoStabilizer::computeCorrelationLocations(uint subframe,
                                                  uint seedX,
                                                  uint seedY,
                                                  uint index,
                                                  uint hFactor,
                                                  uint vFactor){

    for (uint yy = 0; yy < 3; yy++){
        for(uint xx = 0; xx < 3; xx++){
            correlationMatrix[subframe][index].m   = seedX  + (xx*hFactor);
            correlationMatrix[subframe][index++].n = seedY  + (yy*vFactor);
        }
    }
}

void videoStabilizer::stabilizeImage(QImage* imageSrc, QImage* imageDest){
    convertImageToMatrix(imageSrc);
    getGrayCode();
    computeCorrelation();


    populateImageResult(imageDest);
    currentGrayCodeIndex^=1;

    /// TODO: Remove this and uncomment the cleanup line inside computeSubframeCorrelation (...)
    for (uint subframe = 0; subframe < 4; subframe ++){
        for (uint index = 0; index < 9; index++)
            correlationMatrix[subframe][index].value = 0;
    }
    /// END TODO
}



void videoStabilizer::convertImageToMatrix(QImage* imageSrc){
    for (int ii=0; ii < videoHeight; ii++){
        imageMatrix[ii] = imageSrc->scanLine(ii);
    }

}

void videoStabilizer::getGrayCode(){

    for (int subframe = 0; subframe < 4; subframe++){
        getSubframeGrayCode(subframe);
    }
}


void videoStabilizer::getSubframeGrayCode (uchar subframe, BIT_PLANES bitPlane){
    for (uint ii = subframeLocations[subframe].lx - SEARCH_FACTOR_P;
             ii < subframeLocations[subframe].rx + SEARCH_FACTOR_P;
             ii++){
        for (uint jj = subframeLocations[subframe].ly - SEARCH_FACTOR_P;
                 jj < subframeLocations[subframe].ry + SEARCH_FACTOR_P;
                 jj++){
           grayCodeMatrix[currentGrayCodeIndex][jj].setBit(ii, getByteGrayCode(imageMatrix[jj][ii], bitPlane));
        }
    }
}


inline bool videoStabilizer::getByteGrayCode (uchar value, BIT_PLANES bitPlane){

    switch (bitPlane){
    case GC_BP_3:
            return (bool) ((value & GC_BP_3) ^ (value & GC_BP_4));
        break;
    case GC_BP_4:
            return (bool) ((value & GC_BP_4) ^ (value & GC_BP_5));
        break;
    case GC_BP_5:
    default:
            return (bool) ((value & GC_BP_5) ^ (value & GC_BP_6));
        break;
    }


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
    memset(localMinima,0,4*sizeof(tcorrMatElement));

    for (uchar subframe = 0; subframe < 4; subframe++) {
        computeSubframeCorrelation(0, subframe, t_m1);

        computeCorrelationLocations(subframe,
                                    localMinima[subframe].m - SEARCH_FACTOR_P,
                                    localMinima[subframe].n - SEARCH_FACTOR_P,
                                    9,
                                    SEARCH_FACTOR_P,
                                    SEARCH_FACTOR_P);

        getSubframeGrayCode(subframe,GC_BP_4);

        computeSubframeCorrelation(9,subframe,t_m1);
    }
}

void videoStabilizer::computeSubframeCorrelation (uint index, uchar subframe, uchar t_m1){

    localMinima[subframe].value = UINT_MAX;

    for (uint corrIndex = index; corrIndex < (index + 9); corrIndex++){

        computeSingleCorrelation(subframe, t_m1,&correlationMatrix[subframe][corrIndex]);

        // find the minimimum
        if (localMinima[subframe].value > correlationMatrix[subframe][corrIndex].value){

            localMinima[subframe].value = correlationMatrix[subframe][corrIndex].value;
            localMinima[subframe].m = correlationMatrix[subframe][corrIndex].m;
            localMinima[subframe].n = correlationMatrix[subframe][corrIndex].n;
        } // if localMinima

        // cleanup: reset value for next turn around
        //correlationMatrix[subframe][corrIndex].value = 0;
    }
}


inline void videoStabilizer::computeSingleCorrelation (uchar subframe, uchar t_m1, tcorrMatElement* element){
    uint m_offset = element->m - subframeLocations[subframe].lx;
    uint n_offset = element->n - subframeLocations[subframe].ly;

    for (uint x = subframeLocations[subframe].lx;
              x < subframeLocations[subframe].rx;
              x++) {     // x is width
        for (uint y = subframeLocations[subframe].ly;
                  y < subframeLocations[subframe].ry;
                  y++) {  // y is height
            element->value += grayCodeMatrix[currentGrayCodeIndex][y].testBit(x) ^
                              grayCodeMatrix[t_m1][y+n_offset].testBit(x+m_offset);
        }
    }
}
