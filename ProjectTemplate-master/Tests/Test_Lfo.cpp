#include "MyProjectConfig.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "Dsp.h"

SUITE(Lfo)
{
    struct LfoData
    {
        LfoData() : m_pCLfo(0)
        {
            m_fSampleFreq   = 44100;

            m_pfData        = new float [m_iDataLength];
            m_pCLfo         = new CLfo(m_fSampleFreq);
        }

        ~LfoData() 
        {
            delete [] m_pfData;
            delete m_pCLfo;
        }

        CLfo *m_pCLfo;
        static const int m_iDataLength;
        float   m_fSampleFreq;
        float   *m_pfData;
    };
    const int LfoData::m_iDataLength = 17073;

    TEST_FIXTURE(LfoData, LfoSine)
    {
        CSignalGen::generateSine(m_pfData, 13.F, m_fSampleFreq, m_iDataLength, .7F);

        m_pCLfo->setLfoType (CLfo::kSine);

        m_pCLfo->setParam(CLfo::kLfoParamAmplitude, .7F);
        m_pCLfo->setParam(CLfo::kLfoParamFrequency, 13.F);

        for (int i = 0; i < m_iDataLength; i++)
            CHECK_CLOSE(m_pfData[i], m_pCLfo->getNext (), 1e-3F);
    }

    TEST_FIXTURE(LfoData, LfoSaw)
    {
        float fAmplitude = .55F;
        CSignalGen::generateSaw(m_pfData, 21.6F, m_fSampleFreq, m_iDataLength, fAmplitude);

        m_pCLfo->setLfoType (CLfo::kSaw);

        m_pCLfo->setParam(CLfo::kLfoParamAmplitude, fAmplitude);
        m_pCLfo->setParam(CLfo::kLfoParamFrequency, 21.6F);

        m_pCLfo->getNext ();
        for (int i = 1; i < m_iDataLength-1; i++)
        {
            //discard interpolated values at discontinuities
            if (fabsf(m_pfData[i-1]-m_pfData[i]) > fAmplitude || fabsf(m_pfData[i+1]-m_pfData[i]) > fAmplitude)
            {
                m_pCLfo->getNext ();
                continue;
            }
            CHECK_CLOSE(m_pfData[i], m_pCLfo->getNext (), 1e-3F);
        }
    }

    TEST_FIXTURE(LfoData, LfoRect)
    {
        float fAmplitude = .98F;
        CSignalGen::generateRect(m_pfData, 25.2F, m_fSampleFreq, m_iDataLength, fAmplitude);

        m_pCLfo->setLfoType (CLfo::kRect);

        m_pCLfo->setParam(CLfo::kLfoParamAmplitude, fAmplitude);
        m_pCLfo->setParam(CLfo::kLfoParamFrequency, 25.2F);

        m_pCLfo->getNext ();
        for (int i = 1; i < m_iDataLength-1; i++)
        {
            //discard interpolated values at discontinuities
            if (fabsf(m_pfData[i-1]-m_pfData[i]) > fAmplitude || fabsf(m_pfData[i+1]-m_pfData[i]) > fAmplitude)
            {
                m_pCLfo->getNext ();
                continue;
            }
            CHECK_CLOSE(m_pfData[i], m_pCLfo->getNext (), 1e-3F);
        }
    }
}

#endif //WITH_TESTS