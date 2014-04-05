#if !defined(__InputBuffSrcIf_HEADER_INCLUDED__)
#define __InputBuffSrcIf_HEADER_INCLUDED__

#include <algorithm>

#include "ErrorDef.h"

#include "Util.h"

/*! buffer class for faster implementation of variable input block size vs. constant processing block size

    USAGE

        // add new input data of any length
        pInputBuffSrcIf->setDataPtr2Hold(apfTmpPtr, iInpLength);
        while (pInputBuffSrcIf->getBlock(ppfProcBuff, kiProcBuffLength, kiProcBuffLength))
        {
            // do processing on ppfProcBuff with FIXED length kiProcBuffLength          
        }
        // store remaining data
        pInputBuffSrcIf->releaseDataPtr();
 
        // at the end of the processing - get remaining frames from the internal buffer if needed
        pInputBuffSrcIf->flush(ppfProcBuff);
*/

template <class T> 
class CInputBuffSrc
{
public:
    CInputBuffSrc (int iNumChannels, int iMaxOutputLength = 1024, int iInitialLatency = 0) : m_iNumChannels(iNumChannels),
        m_pptInternalBuffer(0),
        m_iMaxInternalBufferLength(iMaxOutputLength-1),
        m_iNumFramesInternalBuffer(iInitialLatency),
        m_pphtExternalData(0),
        m_iExternalDataReadIdx(0),
        m_iNumFramesExternalData(0)
    {
        assert(iInitialLatency >= 0);

        m_pphtExternalData  = new T *[m_iNumChannels];
        m_pptInternalBuffer = new T *[m_iNumChannels];
        for (int i = 0; i < m_iNumChannels; i++)
        {
            m_pptInternalBuffer[i] = new T [m_iMaxInternalBufferLength];
            CUtil::setZero(m_pptInternalBuffer[i], m_iMaxInternalBufferLength);
        }
    };
    virtual ~CInputBuffSrc ()
    {
        if (m_pptInternalBuffer)
        {
            for (int i = 0; i < m_iNumChannels; i++)
                delete [] m_pptInternalBuffer[i];
        }
        delete [] m_pptInternalBuffer;
        delete [] m_pphtExternalData;
    };

    /*! set new external data pointer
    \param T * * ppNewData
    \param int iNumOfElements
    \return Error_t
    */
    Error_t setDataPtr2Hold (T **pptNewData, int iNumFrames)
    {
        if (!pptNewData || iNumFrames <= 0)
            return kFunctionInvalidArgsError;

        for (int i = 0; i < m_iNumChannels; i++)
        {
            assert(pptNewData[i]);
            m_pphtExternalData[i]   = pptNewData[i];
        }

        m_iExternalDataReadIdx      = 0;
        m_iNumFramesExternalData    = iNumFrames;

        return kNoError;
    };
    
    /*! store remaining frames and release external data pointer
    \return Error_t
    */
    Error_t releaseDataPtr ()
    {
        //number of frames to store vs space available
        if (m_iNumFramesExternalData - m_iExternalDataReadIdx > m_iMaxInternalBufferLength - m_iNumFramesInternalBuffer)
            return kFunctionIllegalCallError;

        for (int i = 0; i < m_iNumChannels; i++)
        {
            CUtil::copyBuff(&m_pptInternalBuffer[i][m_iNumFramesInternalBuffer], 
                &m_pphtExternalData[i][m_iExternalDataReadIdx], 
                (m_iNumFramesExternalData - m_iExternalDataReadIdx));
        }

        m_iNumFramesInternalBuffer += m_iNumFramesExternalData - m_iExternalDataReadIdx;
        m_iNumFramesExternalData    = 0;
        m_iExternalDataReadIdx      = 0;

        for (int i = 0; i < m_iNumChannels; i++)
        {
            m_pphtExternalData[i]   = 0;
        }
        return kNoError;
    };
    
    /*! return current block of frames if available
    \param T * * pptBuff
    \param int iOutputBufferSize
    \param int iIncIdxBy
    \return bool
    */
    bool getBlock (T **pptBuff, int iNumFrames2Get, int iIncReadIdxBy = 0)
    {
        int iFramesInExternalBuffer = m_iNumFramesExternalData - m_iExternalDataReadIdx;

        if (iFramesInExternalBuffer + m_iNumFramesInternalBuffer < iNumFrames2Get)
            return false;

        if (!pptBuff || iNumFrames2Get < 0 || iIncReadIdxBy < 0)
            return false;

        // first get the samples from the internal buffer
        int iNumFrames2CopyInt     = std::min(iNumFrames2Get, m_iNumFramesInternalBuffer);
        int iNumFrames2ShiftInt    = std::min(iIncReadIdxBy, iNumFrames2CopyInt);
        for (int i = 0; i < m_iNumChannels; i++)
        {
            assert(pptBuff[i]);
            CUtil::copyBuff(pptBuff[i], m_pptInternalBuffer[i], iNumFrames2CopyInt);
            CUtil::moveBuff(m_pptInternalBuffer[i], 
                0, 
                iNumFrames2ShiftInt, 
                (m_iNumFramesInternalBuffer - iNumFrames2ShiftInt));
        }
        m_iNumFramesInternalBuffer -= iNumFrames2ShiftInt;

        assert(m_iNumFramesInternalBuffer >= 0);
        
        // now get the external data
        int iNumFrames2CopyExt     = std::min(iNumFrames2Get-iNumFrames2CopyInt, iFramesInExternalBuffer);
        int iNumFrames2ShiftExt    = iIncReadIdxBy - iNumFrames2ShiftInt;
        for (int i = 0; i < m_iNumChannels; i++)
        {
             CUtil::copyBuff(&pptBuff[i][iNumFrames2CopyInt], &m_pphtExternalData[i][m_iExternalDataReadIdx], iNumFrames2CopyExt);
        }
        m_iExternalDataReadIdx += iNumFrames2ShiftExt;

        assert(m_iExternalDataReadIdx < m_iNumFramesExternalData);
        assert(m_iExternalDataReadIdx >= 0);

        return true;
    };

    /*! clear all internal buffer and reset member variables
    \return Error_t
    */
    Error_t reset ()
    {
        m_iNumFramesInternalBuffer  = 0;
        m_iNumFramesExternalData    = 0;
        m_iExternalDataReadIdx      = 0;

        for (int i = 0; i < m_iNumChannels; i++)
        {
            m_pphtExternalData[i]   = 0;
            CUtil::setZero(m_pptInternalBuffer[i], m_iMaxInternalBufferLength);
        }

        return kNoError;
    };

    /*! return the remaining frames from internal buffers
    \param T * * pptBuff
    \return int
    */
    int  flush (T **pptBuff)
    {
        int iReturn = m_iNumFramesInternalBuffer;

        if (m_iNumFramesInternalBuffer > 0)
        {
            if (!pptBuff)
                return -1;

            for (int i = 0; i < m_iNumChannels; i++)
            {
                assert(pptBuff[i]);
                CUtil::copyBuff(pptBuff[i], m_pptInternalBuffer[i], m_iNumFramesInternalBuffer);
            }
            m_iNumFramesInternalBuffer  = 0;
        }

        return iReturn;
    };

private:
    int     m_iNumChannels;                 //!< number of channels in data
    T       **m_pptInternalBuffer;          //!< internal buffer
    int     m_iMaxInternalBufferLength;     //!< maximum number of output frames
    int     m_iNumFramesInternalBuffer;     //!< number of frames currently in internal buffer

    T       **m_pphtExternalData;           //!< pointer to external data to be managed
    int     m_iExternalDataReadIdx;         //!< current read index in m_pphtExternalData
    int     m_iNumFramesExternalData;       //!< number of frames in m_pphtExternalData
};


#endif //#define __InputBuffSrcIf_HEADER_INCLUDED__

