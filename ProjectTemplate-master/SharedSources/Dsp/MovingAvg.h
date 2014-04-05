#if !defined(__MovingAvg_hdr__)
#define __MovingAvg_hdr__

#include "ErrorDef.h"
#include "RingBuffer.h"

class CMovingAvg
{
public:
    CMovingAvg(int iMaxFilterLength) :
        m_fPrevSum(0),
        m_pCRingBuff(0)
    {
        m_pCRingBuff = new CRingBuffer<float>(iMaxFilterLength);
        m_pCRingBuff->setWriteIdx(1);
    }
    virtual ~CMovingAvg()
    {
        delete m_pCRingBuff;
    }

    Error_t setFilterLength(int iLength)
    {
        if (iLength > m_pCRingBuff->getLength() || iLength < 0)
            return kFunctionInvalidArgsError;

        m_pCRingBuff->setReadIdx(m_pCRingBuff->getWriteIdx() - iLength);

        m_fPrevSum = 0;
        for (int i = 0; i < iLength; i++)
            m_fPrevSum += m_pCRingBuff->get(static_cast<float>(i));

        return kNoError;
    }

    Error_t process (float *pIn, float *pOut, int iNumSamples)
    {
        if (!pIn || !pOut || iNumSamples < 0)
            return kFunctionInvalidArgsError;

        int iCurrentLength = m_pCRingBuff->getNumValuesInBuffer() ;
        for (int i = 0; i < iNumSamples; i++)
        {
            m_pCRingBuff->putPostInc(pIn[i]);
            m_fPrevSum += pIn[i] - m_pCRingBuff->getPostInc(); 
            pOut[i] = m_fPrevSum / iCurrentLength;
        }

        return kNoError;
    }

private:
    float m_fPrevSum;
    CRingBuffer<float> *m_pCRingBuff;
};
#endif // __MovingAvg_hdr__