#include "MyProjectConfig.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "Dsp.h"

SUITE(MovingAvg)
{
    struct MovingAvgData
    {
        MovingAvgData() : m_pCMovingAvg(0)
        {
            m_pfInputData   = new float [m_iDataLength];
            m_pfOutputData  = new float [m_iDataLength];

            m_pCMovingAvg   = new CMovingAvg(m_iMaxFilterLength);
        }

        ~MovingAvgData() 
        {
            delete [] m_pfInputData;
            delete [] m_pfOutputData;
            delete m_pCMovingAvg;
        }

        CMovingAvg *m_pCMovingAvg;
        static const int m_iDataLength;
        static const int m_iMaxFilterLength;
        float   *m_pfOutputData;
        float   *m_pfInputData;
    };
    const int MovingAvgData::m_iDataLength = 17073;
    const int MovingAvgData::m_iMaxFilterLength = 5013;

    TEST_FIXTURE(MovingAvgData, SimpleSignals)
    {
        CSignalGen::generateDc(m_pfInputData, m_iDataLength, .7F);

        m_pCMovingAvg->setFilterLength (5);
        m_pCMovingAvg->process(m_pfInputData, m_pfOutputData, m_iDataLength);

        CHECK_ARRAY_CLOSE(&m_pfInputData[5], &m_pfOutputData[5], m_iDataLength - 5, 1e-3F);

        m_pCMovingAvg->setFilterLength (2);
        for (int i = 0; i < m_iDataLength; i++)
        {
            if (i%2 ==0)
                m_pfInputData[i]   *= -1;
            m_pCMovingAvg->process(&m_pfInputData[i], &m_pfOutputData[i], 1);

            CHECK_CLOSE(0.F, m_pfOutputData[i], 1e-3F);
        }
    }

    TEST_FIXTURE(MovingAvgData, InputOutput)
    {
        int iCount = 0;
        CUtil::setZero(m_pfInputData, m_iDataLength);

        m_pfInputData[700]  = 1.F;

        m_pCMovingAvg->setFilterLength (501);

        while (iCount < m_iDataLength)
        {
            int iNum = std::min(rand()%(128), m_iDataLength - iCount);

            m_pCMovingAvg->process(&m_pfInputData[iCount], &m_pfOutputData[iCount], iNum);
            iCount += iNum;
        }
        for (int i = 700; i < 700+501; i++)
            CHECK_CLOSE(1.F/501.F, m_pfOutputData[i], 1e-3F);
    }

    TEST_FIXTURE(MovingAvgData, Error)
    {
        CHECK_EQUAL(kFunctionInvalidArgsError, m_pCMovingAvg->process(0, m_pfOutputData, m_iDataLength));
        CHECK_EQUAL(kFunctionInvalidArgsError, m_pCMovingAvg->process(m_pfInputData, 0, m_iDataLength));
        CHECK_EQUAL(kFunctionInvalidArgsError, m_pCMovingAvg->process(m_pfInputData, m_pfOutputData, -1));

        CHECK_EQUAL(kFunctionInvalidArgsError, m_pCMovingAvg->setFilterLength(m_iDataLength+1));
        CHECK_EQUAL(kFunctionInvalidArgsError, m_pCMovingAvg->setFilterLength(-1));

    }
}

#endif //WITH_TESTS