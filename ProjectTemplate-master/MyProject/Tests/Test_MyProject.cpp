#include "MyProjectConfig.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "MyProject.h"

SUITE(MyProject)
{
    struct MyProjectData
    {
        MyProjectData() 
        {
            m_ppfAudioData = new float*[m_iNumChannels];
            m_ppfZeroAudioData = new float*[m_iNumChannels];
            m_ppfTmp = new float*[m_iNumChannels];
            for(int i=0; i<m_iNumChannels; i++)
            {
                m_ppfAudioData[i] = new float[m_iTestDataLength];
                m_ppfZeroAudioData[i] = new float[m_iTestDataLength];
                CUtil::setZero(m_ppfZeroAudioData[i], m_iTestDataLength);
            }
            eWindowType = CFft::kWindowHann;
        }

        ~MyProjectData() 
        {
            assert (m_ppfAudioData != 0 || m_ppfZeroAudioData != 0);
            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfAudioData[i];
                delete [] m_ppfZeroAudioData[i];
            }
            delete [] m_ppfAudioData;
            delete [] m_ppfTmp;
        }
        
        float **m_ppfAudioData;
        float **m_ppfZeroAudioData;
        float **m_ppfTmp;
        CMyProject *pMyFeatExtractor;
        CFft::WindowFunction_t eWindowType;
        
        std::vector<int>m_OptionsArray;
        static const int m_BlockSize;
        static const int m_HopSize;
        static const int m_iTestDataLength;
        static const float m_SampleRate;
        static const int m_iNumChannels;
    };
    
    const int MyProjectData::m_BlockSize  = 1024;
    const int MyProjectData::m_HopSize = 256;
    const int MyProjectData::m_iTestDataLength = 44100;
    const int MyProjectData::m_iNumChannels = 1;
    const float MyProjectData::m_SampleRate = 44100.0;

    TEST_FIXTURE(MyProjectData, ZeroTestForAllFeatures)
    {
        m_OptionsArray.push_back(0);
        m_OptionsArray.push_back(1);
        m_OptionsArray.push_back(2);
        m_OptionsArray.push_back(3); // all features.
        
        int xDim, yDim;
        float **resultMatrix = 0;
        int iNumBlocks = static_cast<int>(floorf((m_iTestDataLength - m_BlockSize)/m_HopSize)) + 1;
        CMyProject::createInstance(pMyFeatExtractor);
        pMyFeatExtractor->initInstance(m_iNumChannels, m_SampleRate, m_BlockSize, m_HopSize, eWindowType, m_OptionsArray, iNumBlocks);
        
        int iNumRemainingFrames = m_iTestDataLength;
        int iNumProcessFrames = 2048; // fixed block size for reading data
        while(iNumRemainingFrames > 0)
        {
            for (int i=0; i<m_iNumChannels; i++)
            {
                m_ppfTmp[i] = &m_ppfZeroAudioData[i][m_iTestDataLength-iNumRemainingFrames];
            }
            
            iNumProcessFrames = std::min(iNumProcessFrames, iNumRemainingFrames);
            pMyFeatExtractor->process(m_ppfTmp, m_ppfTmp, iNumProcessFrames);
            iNumRemainingFrames -= iNumProcessFrames;
        }
        
        
        pMyFeatExtractor->getSizeOfResult(xDim, yDim);
        CHECK_EQUAL(xDim, 4);
        CHECK_EQUAL(yDim, iNumBlocks);
        pMyFeatExtractor->getResult(resultMatrix);
        for (int i=0; i<yDim; i++) // block index
        {
            for(int j=0; j<xDim; j++) // feature index
            {
                CHECK_CLOSE(0.0, resultMatrix[j][i], 1e-3);
            }
        }
        CMyProject::destroyInstance(pMyFeatExtractor);
        resultMatrix = 0;
        m_OptionsArray.clear();
    }
}

#endif //WITH_TESTS