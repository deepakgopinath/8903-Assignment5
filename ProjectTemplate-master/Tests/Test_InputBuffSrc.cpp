#include "MyProjectConfig.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "SignalGen.h"

#include "InputBuffSrc.h"

SUITE(InputBuffSrc)
{
    struct InputBuffSrcData
    {
        InputBuffSrcData()  
        {
            float fPhase = 0.F;
            m_pCInputBuffSrc = new CInputBuffSrc<float>(m_iNumChannels, m_iMaxOutputLength, m_iInitialLatency);
            m_ppfAudioData  = new float*[m_iNumChannels];
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_ppfAudioData[i]   = new float[m_iBuffLength];
            }
            for (int i = 0; i < m_iNumChannels; i++)
            {
                CSignalGen::generateSine(m_ppfAudioData[i], 441.F, 44100.F, m_iBuffLength, .6F, fPhase);
                fPhase  += static_cast<float>(M_PI_2);
            }

        }

        ~InputBuffSrcData() 
        {
            delete m_pCInputBuffSrc;
            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfAudioData[i];
            }
            delete [] m_ppfAudioData;
}


        float **m_ppfAudioData;
        static const int m_iNumChannels;
        static const int m_iBuffLength;
        static const int m_iInitialLatency;
        static const int m_iMaxOutputLength;
        CInputBuffSrc<float> *m_pCInputBuffSrc;

    };
    const int InputBuffSrcData::m_iNumChannels = 3;
    const int InputBuffSrcData::m_iBuffLength =  2000;
    const int InputBuffSrcData::m_iInitialLatency =  20;
    const int InputBuffSrcData::m_iMaxOutputLength = 512;

    TEST_FIXTURE(InputBuffSrcData, ZeroInput)
    {
        delete m_pCInputBuffSrc;
        m_pCInputBuffSrc = new CInputBuffSrc<float>(1, m_iMaxOutputLength, m_iInitialLatency);
        
        static const int iDataLength    = 358;
        static const int iHopLength     = 512;
        float* outputBuffer=new float[2*iDataLength + m_iInitialLatency];
        CUtil::setZero(m_ppfAudioData[0], 358);
        m_pCInputBuffSrc->setDataPtr2Hold(m_ppfAudioData, iDataLength );    
        m_pCInputBuffSrc->releaseDataPtr();
        m_pCInputBuffSrc->setDataPtr2Hold(m_ppfAudioData, iDataLength );    
        m_pCInputBuffSrc->getBlock(&outputBuffer, 2*iDataLength + m_iInitialLatency, iHopLength);
        for(int i=0; i< 2*iDataLength + m_iInitialLatency; i++){
            CHECK_EQUAL(0, outputBuffer[i]);

        }

        m_pCInputBuffSrc->releaseDataPtr();
        CHECK_EQUAL(2*iDataLength + m_iInitialLatency - iHopLength, m_pCInputBuffSrc->flush(&outputBuffer));
        
        delete [] outputBuffer;
    }

    TEST_FIXTURE(InputBuffSrcData, FlushTest)
    {
        float **ppBuffer = new float *[m_iNumChannels];
        int kiProcBuffLength = 300; // blocklength

        for (int i = 0; i<m_iNumChannels; i++)
            ppBuffer[i] = new float[kiProcBuffLength];

        m_pCInputBuffSrc->setDataPtr2Hold(m_ppfAudioData, m_iBuffLength);
        while (m_pCInputBuffSrc->getBlock(ppBuffer, kiProcBuffLength, kiProcBuffLength))
        {
        }
        m_pCInputBuffSrc->releaseDataPtr();
        CHECK_EQUAL(m_iBuffLength%kiProcBuffLength+m_iInitialLatency, m_pCInputBuffSrc->flush(ppBuffer));
        for(int i=0; i<m_iNumChannels; i++)
        {
            CHECK_ARRAY_CLOSE(&m_ppfAudioData[i][m_iBuffLength-(m_iBuffLength%kiProcBuffLength)], 
                &ppBuffer[i][m_iInitialLatency], 
                m_iBuffLength%kiProcBuffLength, 
                1e-3);
        }

        for (int i = 0; i<m_iNumChannels; i++)
            delete [] ppBuffer[i];
        delete [] ppBuffer;

    }

    TEST_FIXTURE(InputBuffSrcData, DiffChannels)
    {
        CInputBuffSrc<float> *pCTest1 = new CInputBuffSrc<float>(3,m_iMaxOutputLength/2);
        CInputBuffSrc<float> *pCTest2 = new CInputBuffSrc<float>(2,m_iMaxOutputLength/2);
        float ** ppfInput = new float * [3];
        float ** ppfOutput = new float*[3];
        int iReadIdx = 0;

        for (int i = 0 ; i < 3 ; i++) {
            ppfInput[i] = new float[m_iMaxOutputLength*2];
            CSignalGen::generateSine(ppfInput[i], 20.F, 1, m_iMaxOutputLength*2, .7F, static_cast<float>(i*M_PI_2));
            ppfOutput[i] = new float [17];
        }
        Error_t t = pCTest1->setDataPtr2Hold(ppfInput, m_iMaxOutputLength*2);
        CHECK_EQUAL(t, kNoError);
        while (pCTest1->getBlock(ppfOutput, 17, 17)) {
            for (int i = 0 ; i < 3; i++) {
                CHECK_ARRAY_CLOSE(&ppfInput[i][iReadIdx], 
                    ppfOutput[i], 
                    17, 
                    1e-3);
            }
            iReadIdx += 17;
        }

        for (int i = 0 ; i < 3 ; i++) {
            delete ppfOutput[i];
        }
        delete ppfOutput;
        ppfOutput = new float* [2];
        for (int i = 0 ; i < 2 ; i++) {
            ppfOutput[i] = new float [17];
        }
        Error_t tt = pCTest2 -> setDataPtr2Hold(ppfInput, m_iMaxOutputLength*2);
        CHECK_EQUAL(tt, kNoError);
        iReadIdx = 0;
        while (pCTest2->getBlock(ppfOutput, 17, 17)) {
            for (int i = 0 ; i < 2; i++) {
                CHECK_ARRAY_CLOSE(&ppfInput[i][iReadIdx], 
                    ppfOutput[i], 
                    17, 
                    1e-3);
            }
            iReadIdx += 17;
        }
        for (int i = 0 ; i < 2 ; i++) {
            delete [] ppfOutput[i];
        }
        delete [] ppfOutput;
        for (int i = 0 ; i < 3 ; i++) {
            delete [] ppfInput[i];
        }
        delete [] ppfInput;
    }

    TEST_FIXTURE(InputBuffSrcData, Flush)
    {
        float **ppBlock = new float* [m_iNumChannels];
        for (int i = 0; i < m_iNumChannels; i++) {
            ppBlock[i] = new float [m_iBuffLength];
        }

        int iOffset = -1;

        m_pCInputBuffSrc->setDataPtr2Hold(m_ppfAudioData, m_iBuffLength);
        while (m_pCInputBuffSrc->getBlock(ppBlock, m_iBuffLength, m_iBuffLength))
        {
            for (int c = 0; c < m_iNumChannels; c++)
            {
                CHECK_ARRAY_CLOSE(m_ppfAudioData[c], &ppBlock[c][m_iInitialLatency], m_iBuffLength-m_iInitialLatency, 1e-3F);
            }
        }
        m_pCInputBuffSrc->releaseDataPtr();
        iOffset = m_pCInputBuffSrc->flush(ppBlock);
        for (int c = 0; c < m_iNumChannels; c++)
        {
            CHECK_ARRAY_CLOSE(&m_ppfAudioData[c][m_iBuffLength - m_iInitialLatency], ppBlock[c], m_iInitialLatency, 1e-3F);
        }
        // internal buffer iOffset set to zero
        // remaining samples successfully obtained
        CHECK_EQUAL(m_iInitialLatency, iOffset);
 
        // internal buffer cleaned
        iOffset = m_pCInputBuffSrc->flush(ppBlock);
        CHECK_EQUAL(0, iOffset);
    }

    TEST_FIXTURE(InputBuffSrcData, VaryBlocksize)
    {
        srand(17);
        float ** ppfOutput;
        ppfOutput = new float* [m_iNumChannels];
        for(int i = 0 ; i < m_iNumChannels ; i ++){
            ppfOutput[i] = new float [m_iMaxOutputLength];
        }
        int iOffset = 0;
        int iNum = rand()%(m_iMaxOutputLength);
        Error_t t = m_pCInputBuffSrc -> setDataPtr2Hold(m_ppfAudioData, m_iBuffLength);
        CHECK_EQUAL(t, kNoError);
        while (m_pCInputBuffSrc->getBlock(ppfOutput, iNum, iNum)) 
        {
            for (int c = 0; c < m_iNumChannels; c++)
            {
                CHECK_ARRAY_CLOSE(&m_ppfAudioData[c][iOffset], &ppfOutput[c][m_iInitialLatency], iNum-m_iInitialLatency, 1e-3F);
            }
            iOffset += iNum;
            iNum = rand()%(m_iMaxOutputLength);
        }
        m_pCInputBuffSrc->releaseDataPtr();
        m_pCInputBuffSrc->flush(ppfOutput);


        iOffset = 0;
        int iWriteIdx = 0;
        float **ppfTmpIn = new float *[m_iNumChannels];
        while (iWriteIdx < m_iBuffLength)
        {
            iNum = std::min(rand()%(m_iBuffLength), m_iBuffLength - iWriteIdx);

            for (int c = 0; c < m_iNumChannels; c++)
            {
                ppfTmpIn[c]  = &m_ppfAudioData[c][iWriteIdx];
            }
            t = m_pCInputBuffSrc->setDataPtr2Hold(ppfTmpIn, iNum);
            CHECK_EQUAL(t, kNoError);
            while (m_pCInputBuffSrc->getBlock(ppfOutput, m_iMaxOutputLength,m_iMaxOutputLength)) {
                for (int c = 0; c < m_iNumChannels; c++)
                {
                    CHECK_ARRAY_CLOSE(&m_ppfAudioData[c][iOffset], ppfOutput[c], m_iMaxOutputLength, 1e-3F);
                }
                iOffset += m_iMaxOutputLength;
            }
            m_pCInputBuffSrc->releaseDataPtr();
            iWriteIdx   += iNum;

        }

        CHECK_EQUAL(m_iBuffLength - iOffset, m_pCInputBuffSrc->flush(ppfOutput));

        for(int i = 0 ; i < m_iNumChannels ; i ++)
        {
            delete [] ppfOutput[i];
        }
        delete [] ppfTmpIn;
        delete [] ppfOutput;
    }
    TEST_FIXTURE(InputBuffSrcData, ErrorCodes)
    {
        CHECK_EQUAL(m_iInitialLatency, m_pCInputBuffSrc->flush(m_ppfAudioData));
        CHECK_EQUAL(0, m_pCInputBuffSrc->flush(m_ppfAudioData));

        CHECK_EQUAL(kFunctionInvalidArgsError, m_pCInputBuffSrc->setDataPtr2Hold(0, 1));
        CHECK_EQUAL(kNoError, m_pCInputBuffSrc->releaseDataPtr());
        
        CHECK_EQUAL(false, m_pCInputBuffSrc->getBlock(0, 1024));
        CHECK_EQUAL(false, m_pCInputBuffSrc->getBlock(m_ppfAudioData, m_iMaxOutputLength));

        CHECK_EQUAL(kNoError, m_pCInputBuffSrc->setDataPtr2Hold(m_ppfAudioData, m_iBuffLength));
        CHECK_EQUAL(kFunctionIllegalCallError, m_pCInputBuffSrc->releaseDataPtr());

        CHECK_EQUAL(false, m_pCInputBuffSrc->getBlock(m_ppfAudioData, 10000));
        CHECK_EQUAL(false, m_pCInputBuffSrc->getBlock(0, m_iMaxOutputLength));
        CHECK_EQUAL(false, m_pCInputBuffSrc->getBlock(m_ppfAudioData, -1));

        for (int i = 0; i < 3; i++)
            CHECK_EQUAL(true, m_pCInputBuffSrc->getBlock(m_ppfAudioData, m_iMaxOutputLength, m_iMaxOutputLength));
        CHECK_EQUAL(kNoError, m_pCInputBuffSrc->releaseDataPtr());

        CHECK_EQUAL(false, m_pCInputBuffSrc->getBlock(m_ppfAudioData, m_iMaxOutputLength, m_iMaxOutputLength));
        CHECK_EQUAL(kNoError, m_pCInputBuffSrc->releaseDataPtr());

        CHECK_EQUAL(kNoError, m_pCInputBuffSrc->reset());
    }
}

#endif //WITH_TESTS




