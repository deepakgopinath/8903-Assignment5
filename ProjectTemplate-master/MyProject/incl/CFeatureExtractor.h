//
//  CFeatureExtractor.h
//  MyProject
//
//  Created by Deepak on 4/6/14.
//
//

#ifndef __MyProject__CFeatureExtractor__
#define __MyProject__CFeatureExtractor__

#include <iostream>
#include "ErrorDef.h"



class CFeatureExtractor
{
public:
    enum SpectralFeatureList
    {
        kSpectralCentroid, // 0
        kSpectralFlux, // 1
        kSpectralRolloff, // 2
        
        kNumSpectralFeatures // 3
    };
    
    enum TimeDomainFeatureList
    {
        kZeroCrossRate = kNumSpectralFeatures, // 3
        kTotalNumFeatures // 4
    };

    CFeatureExtractor(float fSampleRate, int iFftSize, float fKappa);
    virtual ~CFeatureExtractor();
    
    float extractFeatures(float *inputData, int sizeOfData, int featureOptionSelected);
private:
    
    float m_fSampleRate;
    float *m_pfPrevBlock;
    float m_fKappa;
    
    float spectralCentroid(float *spectralData, int sizeOfData);
    float spectralFlux(float *spectralData, int sizeOfData);
    float spectralRollOff(float *spectralData, int sizeOfData, float fKappa = 0.85);
    float zeroCrossingRate(float *temporalData, int sizeOfData);
    
};

#endif /* defined(__MyProject__CFeatureExtractor__) */
