#include "MyProjectConfig.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "MyProject.h"
#include "AudioFileIf.h"
#include <vector>
#include <fstream.h>
#include <iomanip.h>
#include "Util.h"

extern std::string cTestDataDir;

SUITE(MyProject)
{
    struct MyProjectData
    {
        MyProjectData() 
        {
            const std::string cExt(".wav");
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
            eWindow = CFft::kPreWindow;
            CAudioFileIf::createInstance(pMyAudioFile);
            pMyAudioFile->openFile(cTestDataDir + "/foxShort" + cExt, CAudioFileIf::kFileRead);
            pMyAudioFile->getFileSpec(stFileSpec);
            pMyAudioFile->getLength(iNumFramesInAudioFile);
            // std::cout<<iNumFramesInAudioFile<<std::endl;
            iNumBlocks = static_cast<int>(floorf((iNumFramesInAudioFile - m_BlockSize)/m_HopSize)) + 1;
            
            std::ifstream myMatlabRes;
            myMatlabRes.open((cTestDataDir + "/matlabrefCentroid.txt").c_str());
            std::string str;
            if(myMatlabRes.is_open())
            {
                while(myMatlabRes.good())
                {
                    getline(myMatlabRes, str);
                    const char *temp = str.c_str();
                    m_MatlabCentroid.push_back(atof(temp));
                }
            }
            myMatlabRes.close();
            str.clear();
            
            myMatlabRes.open((cTestDataDir + "/matlabrefFlux.txt").c_str());
            if(myMatlabRes.is_open())
            {
                while(myMatlabRes.good())
                {
                    getline(myMatlabRes, str);
                    const char *temp = str.c_str();
                    //std::cout << std::setprecision(9) << atof(temp) <<std::endl;
                    m_MatlabFlux.push_back(atof(temp));
                }
            }
            myMatlabRes.close();
            str.clear();
            
            myMatlabRes.open((cTestDataDir + "/matlabrefRolloff.txt").c_str());
            if(myMatlabRes.is_open())
            {
                while(myMatlabRes.good())
                {
                    getline(myMatlabRes, str);
                    const char *temp = str.c_str();
                    //std::cout << std::setprecision(13) << atof(temp) <<std::endl;
                    m_MatlabRolloff.push_back(atof(temp));
                }
            }
            myMatlabRes.close();
            str.clear();
            
            myMatlabRes.open((cTestDataDir + "/matlabrefZCR.txt").c_str());
            if(myMatlabRes.is_open())
            {
                while(myMatlabRes.good())
                {
                    getline(myMatlabRes, str);
                    const char *temp = str.c_str();
                  //  std::cout << std::setprecision(13) << atof(temp) <<std::endl;
                    m_MatlabZCR.push_back(atof(temp));
                }
            }
            myMatlabRes.close();
            str.clear();
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
        CAudioFileIf *pMyAudioFile;
        CAudioFileIf::FileSpec_t stFileSpec;
        CFft::WindowFunction_t eWindowType;
        CFft::Windowing_t eWindow;
        int iNumBlocks;
        long long iNumFramesInAudioFile;
        
        std::vector<int>m_OptionsArray;
        std::vector<float> m_MatlabCentroid;
        std::vector<float> m_MatlabFlux;
        std::vector<float> m_MatlabRolloff;
        std::vector<float> m_MatlabZCR;
        
        static const int m_BlockSize;
        static const int m_HopSize;
        static const int m_iTestDataLength;
        static const float m_SampleRate;
        static const int m_iNumChannels;
    };
    
    const int MyProjectData::m_BlockSize  = 256;
    const int MyProjectData::m_HopSize = 128;
    const int MyProjectData::m_iTestDataLength = 1024;
    const int MyProjectData::m_iNumChannels = 1;
    const float MyProjectData::m_SampleRate = 8192.0;

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
        pMyFeatExtractor->initInstance(m_iNumChannels, m_SampleRate, m_BlockSize, m_HopSize, eWindowType, eWindow, 0.85, m_OptionsArray, iNumBlocks);
        
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
                if(j != 2)
                {
                    CHECK_CLOSE(0.0, resultMatrix[j][i], 1e-3);
                }
                else
                {
                    CHECK_CLOSE((m_SampleRate/2.0)/((m_BlockSize/2.0) + 1), resultMatrix[j][i], 1e-3);
                }
                
            }
        }
        CMyProject::destroyInstance(pMyFeatExtractor);
        resultMatrix = 0;
        m_OptionsArray.clear();
    }

    TEST_FIXTURE(MyProjectData, RegressionTestsForZeroCrossFluxFeatures)
    {
        m_OptionsArray.push_back(0);
        m_OptionsArray.push_back(1);
        m_OptionsArray.push_back(2);
        m_OptionsArray.push_back(3); // all features.
        const int kBlockSize = 1024;
        int xDim, yDim;
        float **resultMatrix = 0;
        CMyProject::createInstance(pMyFeatExtractor);
        pMyFeatExtractor->initInstance(stFileSpec.iNumChannels, stFileSpec.fSampleRateInHz, m_BlockSize,m_HopSize, CFft::kWindowHann, eWindow, 0.85, m_OptionsArray, iNumBlocks);
        float ** ppfAudioData = 0;
        ppfAudioData            = new float* [stFileSpec.iNumChannels];
        for (int i = 0; i < stFileSpec.iNumChannels; i++)
            ppfAudioData[i] = new float [kBlockSize];
        while (!pMyAudioFile->isEof())
        {
            int iNumFrames = kBlockSize;
            pMyAudioFile->readData(ppfAudioData, iNumFrames);
            pMyFeatExtractor->process(ppfAudioData, ppfAudioData, iNumFrames);
        }
        
        pMyFeatExtractor->getSizeOfResult(xDim, yDim);
        
        CHECK_EQUAL(xDim, 4);
        CHECK_EQUAL(yDim, iNumBlocks);
        pMyFeatExtractor->getResult(resultMatrix);
        //Now compare the elements of this result with the ones in the text file.
        
        for (int i=0; i<yDim; i++) // block index
        {
            if(i !=0)
            {
                CHECK_CLOSE(fabs(1.0 - m_MatlabFlux[i]/resultMatrix[1][i]), 0.0, 6e-2);
            }
            CHECK_CLOSE(fabs(resultMatrix[3][i] - m_MatlabZCR[i]), 0.0, 1e-5); // this will stay.
        }
        
        for(int i=0; i<stFileSpec.iNumChannels; i++)
        {
            delete [] ppfAudioData[i];
        }
        delete [] ppfAudioData;
        CMyProject::destroyInstance(pMyFeatExtractor);
        resultMatrix = 0;
        m_OptionsArray.clear();

        
        
    }
    
    TEST_FIXTURE(MyProjectData, SpectralCentroidSine)
    {
        m_OptionsArray.push_back(0);
        float fSineFreq = 128.0;
        CMyProject::createInstance(pMyFeatExtractor);
        pMyFeatExtractor->initInstance(m_iNumChannels, m_SampleRate, m_iTestDataLength, m_HopSize, eWindowType, CFft::kNoWindow, 0.85, m_OptionsArray, 1); // set blocksize to be same as the total length of file...so that there is only one spectral centroid value returned for the entire file.. No windowing is used..
        float **mSineAudio;
        mSineAudio = new float*[m_iNumChannels];
        for(int i=0; i<m_iNumChannels; i++)
        {
            mSineAudio[i] = new float[m_iTestDataLength];
            CSignalGen::generateSine(mSineAudio[i], fSineFreq, m_SampleRate, m_iTestDataLength);
        }
        
        int iNumRemainingFrames = m_iTestDataLength;
        int iNumProcessFrames = 512; // fixed block size for reading data
        while(iNumRemainingFrames > 0)
        {
            for (int i=0; i<m_iNumChannels; i++)
            {
                m_ppfTmp[i] = &mSineAudio[i][m_iTestDataLength-iNumRemainingFrames];
            }
            
            iNumProcessFrames = std::min(iNumProcessFrames, iNumRemainingFrames);
            pMyFeatExtractor->process(m_ppfTmp, m_ppfTmp, iNumProcessFrames);
            iNumRemainingFrames -= iNumProcessFrames;
        }
        int xDim, yDim;
        float **resultMatrix = 0;
        
        pMyFeatExtractor->getSizeOfResult(xDim, yDim);
        CHECK_EQUAL(xDim, 1);
        CHECK_EQUAL(yDim, 1);
        pMyFeatExtractor->getResult(resultMatrix);
        for (int i=0; i<yDim; i++) // block index
        {
            for(int j=0; j<xDim; j++) // feature index
            {
                CHECK_CLOSE((1.0 - resultMatrix[j][i]/fSineFreq), 0.0, 2e-3) ;
            }
        }
        
        for(int i=0; i<m_iNumChannels; i++)
        {
            delete [] mSineAudio[i];
        }
        delete [] mSineAudio;
        m_OptionsArray.clear();
    }
    
    TEST_FIXTURE(MyProjectData, SpectralRollOff)
    {
        m_OptionsArray.push_back(2);
        float **mSineAudio = 0;
        mSineAudio = new float*[m_iNumChannels];
        float *tempArray = new float[m_iTestDataLength];
        float fSineFreq = 8.0;
        
        CMyProject::createInstance(pMyFeatExtractor);
        pMyFeatExtractor->initInstance(m_iNumChannels, m_SampleRate, m_iTestDataLength, m_HopSize, eWindowType, CFft::kNoWindow, 0.8, m_OptionsArray, 1); // kappa of 0.8
        
        int indexOfFourthOctave = 0;
        float binSize = (m_SampleRate/2.0)/((m_iTestDataLength/2.0) + 1.0);
        for(int i=0; i<m_iNumChannels; i++)
        {
            mSineAudio[i] = new float[m_iTestDataLength];
            for(int j=0; j < 5; j++) // adding 4 octaves of 8Hz
            {
                CSignalGen::generateSine(tempArray, fSineFreq*powf(2.0, static_cast<float>(j)), m_SampleRate, m_iTestDataLength, 0.2);
                CUtil::addBuff(mSineAudio[i], tempArray, m_iTestDataLength);
                if (j == 3)
                {
                    indexOfFourthOctave = static_cast<int>(ceilf(fSineFreq*powf(2.0, static_cast<float>(j))/binSize)); // this is the bin index to which 64Hz belong to.
                }
            }
        }
        int iNumRemainingFrames = m_iTestDataLength;
        int iNumProcessFrames = 512; // fixed block size for reading data
        while(iNumRemainingFrames > 0)
        {
            for (int i=0; i<m_iNumChannels; i++)
            {
                m_ppfTmp[i] = &mSineAudio[i][m_iTestDataLength-iNumRemainingFrames];
            }
            
            iNumProcessFrames = std::min(iNumProcessFrames, iNumRemainingFrames);
            pMyFeatExtractor->process(m_ppfTmp, m_ppfTmp, iNumProcessFrames);
            iNumRemainingFrames -= iNumProcessFrames;
        }
        int xDim, yDim;
        float **resultMatrix = 0;
        
        pMyFeatExtractor->getSizeOfResult(xDim, yDim);
        CHECK_EQUAL(xDim, 1);
        CHECK_EQUAL(yDim, 1);
        pMyFeatExtractor->getResult(resultMatrix);
        for (int i=0; i<yDim; i++) // block index
        {
            for(int j=0; j<xDim; j++) // feature index
            {
                CHECK_CLOSE(indexOfFourthOctave*binSize - resultMatrix[j][i], 0.0, 1e-3);
            }
        }
        
        for(int i=0; i<m_iNumChannels; i++)
        {
            delete [] mSineAudio[i];
        }
        delete [] mSineAudio;
        delete [] tempArray;
        CMyProject::destroyInstance(pMyFeatExtractor);
        m_OptionsArray.clear();
        
        

    }
}

#endif //WITH_TESTS