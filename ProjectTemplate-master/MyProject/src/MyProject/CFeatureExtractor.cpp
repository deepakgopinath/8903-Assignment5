//
//  CFeatureExtractor.cpp
//  MyProject
//
//  Created by Deepak on 4/6/14.
//
//

#include "CFeatureExtractor.h"
#include "SignalGen.h"
#include <math.h>

CFeatureExtractor::CFeatureExtractor(float fSampleRate, int iFftSize, float fKappa):
                    m_fSampleRate(fSampleRate),
                    m_pfPrevBlock(0),
                    m_fKappa(fKappa)
{
    m_pfPrevBlock = new float[iFftSize];
    CSignalGen::generateDc(m_pfPrevBlock, iFftSize, -1.0f);
}

CFeatureExtractor::~CFeatureExtractor()
{
    delete [] m_pfPrevBlock;
    m_pfPrevBlock = 0;
}

float CFeatureExtractor::extractFeatures(float *inputData, int sizeOfData, int featureOptionSelected)
{
    
    switch (featureOptionSelected) {
        case kSpectralCentroid:
            return spectralCentroid(inputData, sizeOfData);
            break;
        case kSpectralFlux:
            return spectralFlux(inputData, sizeOfData);
            break;
        case kSpectralRolloff:
            return spectralRollOff(inputData, sizeOfData, m_fKappa); // hard coded kappa value
            break;
        case kZeroCrossRate:
            return zeroCrossingRate(inputData, sizeOfData);
            break;
        default:
            break;
    }
    
    return -1.0;
}

float CFeatureExtractor::spectralCentroid(float *spectralData, int sizeOfData)
{
    float fSpectralCentroid = 0.0;
    float fNumSum = 0.0;
    float fDenSum = 0.0;
    
    for (int i = 0; i < sizeOfData; i++)
    {
        fNumSum += i * powf(spectralData[i], 2.0);
        fDenSum += powf(spectralData[i], 2.0);
    }
    
    if (fDenSum == 0.0)
    {
        fSpectralCentroid = 0.0;
    }
    else
    {
        fSpectralCentroid = fNumSum/fDenSum;
    }
    
    fSpectralCentroid = (fSpectralCentroid/static_cast<float>(sizeOfData))*(m_fSampleRate/2.0);
    return fSpectralCentroid;
}

float CFeatureExtractor::spectralFlux(float *spectralData, int sizeOfData)
{
    float fSpectralFlux = 0.0;
    float fNum = 0.0;
    
    if (m_pfPrevBlock[0] == -1.0) {
        memcpy(m_pfPrevBlock, spectralData, sizeOfData*sizeof(float));
    }
    
    for (int i = 0; i < sizeOfData; i++) {
        fNum += powf((spectralData[i] - m_pfPrevBlock[i]), 2.0);
    }
    
    fNum = sqrtf(fNum);
    fSpectralFlux = (fNum*2.0)/(sizeOfData); // why the heck do we have to multiply by 2.0 to get the same answer as matlab!!!
    
    memcpy(m_pfPrevBlock, spectralData, sizeOfData*sizeof(float));
    
    return fSpectralFlux;
}

float CFeatureExtractor::spectralRollOff(float *spectralData, int sizeOfData, float fKappa)
{
    float fSpectralRollOff = 0.0;
    float fTotalSum = 0.0;
    float fCumSum = 0.0;
    
    for (int i = 0; i < sizeOfData; i++) {
        fTotalSum += spectralData[i];
    }
    
    for (int i = 0; i < sizeOfData; i++) {
        fCumSum += spectralData[i];
        
        //std::cout << fCumSum - fKappa*fTotalSum << std::endl;
        
        if (fCumSum >= fKappa*fTotalSum ) {
            fSpectralRollOff = i+1;
            //fSpectralRollOff = ((i+1) * m_fSampleRate)/(2.0*sizeOfData); // i+1 because the we need the bin index...
            break;
        }
    }
    fSpectralRollOff = (fSpectralRollOff/static_cast<float>(sizeOfData)) * (m_fSampleRate/2.0);
    return fSpectralRollOff;
}

float CFeatureExtractor::zeroCrossingRate(float *temporalData, int sizeOfData)
{
    float fZeroCrossingRate = 0.0;
    float fSumOfDiff = 0.0;
    
    for(int i=0; i< sizeOfData; i++)
    {
        if(temporalData[i] != 0.0)
        {
            temporalData[i] = temporalData[i]/fabsf(temporalData[i]); // get the sign 1, 0 or -1
        }
    }
    
    for(int i = 0; i < sizeOfData - 1; i++)
    {
        temporalData[i] = fabsf(temporalData[i+1] - temporalData[i]);
        fSumOfDiff += temporalData[i];
        
    }
    
    fZeroCrossingRate = 0.5 * (fSumOfDiff/static_cast<float>(sizeOfData - 1));
    
    return fZeroCrossingRate;
    
}