
#include <cassert>
#include <iostream>

#include "MyProjectConfig.h"
// 3rd party includes
#ifdef WITH_SNDLIB
#include "sndlib.h"
#endif //WITH_SNDLIB


#include "Util.h"

#include "AudioFileIf.h"

using namespace std;

// consts
const int CAudioFileIf::m_kiDefBlockLength      = 1024;

Error_t CAudioFileIf::createInstance( CAudioFileIf*& pCInstance )
{
#ifdef WITH_SNDLIB
    pCInstance   = new CAudioFileSndLib ();
#else
    pCInstance   = new CAudioFileRaw ();
#endif

    if (!pCInstance)
        return kMemError;

    return kNoError;
}

Error_t CAudioFileIf::destroyInstance( CAudioFileIf*& pCInstance )
{
    delete pCInstance;
    pCInstance  = 0;

    return kNoError;
}

CAudioFileIf::CAudioFileIf() : 
    m_eIoType(kFileRead),
    m_bWithClipping(true),
    m_bIsInitialized(false),
    m_iNumBytesPerSample(2)
{
}

CAudioFileIf::~CAudioFileIf()
{
}

Error_t CAudioFileIf::resetInstance( bool bFreeMemory /*= false*/ )
{
    Error_t eErr = closeFile ();
    if (eErr != kNoError)
        return eErr;

    if (bFreeMemory)
    {
        eErr = freeMemory ();
        if (eErr != kNoError)
            return eErr;
    }
  
    eErr = initDefaults ();
    if (eErr != kNoError)
        return eErr;

    return eErr;
}

Error_t CAudioFileIf::freeMemory()
{
    return kNoError;
}

Error_t CAudioFileIf::allocMemory()
{
    freeMemory ();

    return kNoError;
}

Error_t CAudioFileIf::initDefaults()
{
    m_sCurrFileSpec.eBitStreamType  = kFileBitStreamInt16;
    m_sCurrFileSpec.eFormat         = kFileFormatRaw;
    m_sCurrFileSpec.fSampleRateInHz = 48000;
    m_sCurrFileSpec.iNumChannels    = 2;

    setIoType(kFileRead);

    setInitialized(false);
    setClippingEnabled ();

    return kNoError;
}

Error_t CAudioFileIf::readData( float **ppfAudioData, int &iLength )
{
    // check parameters
    if (!ppfAudioData || !ppfAudioData[0] || iLength < 0)
        return kFunctionInvalidArgsError;

    // check file status
    if (!isOpen())
        return kUnknownError;

    // check file properties
    if (!isInitialized())
        return kNotInitializedError;

    // update iLength to the number of frames actually read
    iLength = readDataIntern (ppfAudioData, iLength);
    if (iLength < 0)
        return kFileAccessError;

    return kNoError;
}

Error_t CAudioFileIf::writeData( float **ppfAudioData, int iLength )
{

    // check parameters
    if (!ppfAudioData || !ppfAudioData[0] || iLength < 0)
        return kFunctionInvalidArgsError;

    // check file status
    if (!isOpen())
        return kUnknownError;

    // check file properties
    if (!isInitialized())
        return kNotInitializedError;

    // update iLength
    iLength = writeDataIntern (ppfAudioData, iLength);
    if (iLength < 0)
        return kFileAccessError;

    return kNoError;
}

long long CAudioFileIf::convFrames2Bytes( long long iNumFrames )
{
    return m_iNumBytesPerSample*iNumFrames*getNumChannels();
}

long long CAudioFileIf::convBytes2Frames( long long iNumBytes )
{
    return iNumBytes/(m_iNumBytesPerSample * getNumChannels());    
}

Error_t CAudioFileIf::getFileSpec( FileSpec_t &sFileSpec )
{
    memcpy (&sFileSpec, &m_sCurrFileSpec, sizeof(FileSpec_t));

    return kNoError;
}

Error_t CAudioFileIf::setClippingEnabled( bool bIsEnabled /*= true*/ )
{
    m_bWithClipping = bIsEnabled;
    return kNoError;
}

Error_t CAudioFileIf::setPosition( long long iFrame /*= 0*/ )
{
    // check file status
    if (!isOpen())
        return kUnknownError;

    // check file properties
    if (!isInitialized())
        return kNotInitializedError;

    if (iFrame <= 0 || iFrame >= getLengthIntern())
        return kFunctionInvalidArgsError;

    return setPositionIntern(iFrame);

}

Error_t CAudioFileIf::setPosition( double dTimeInS /*= .0*/ )
{
    long long iPosInFrames = CUtil::double2int<long long>(dTimeInS * m_sCurrFileSpec.fSampleRateInHz);

    return setPosition (iPosInFrames);
}

Error_t CAudioFileIf::getLength( long long &iLengthInFrames ) 
{
    iLengthInFrames = -1;

    // check file status
    if (!isOpen())
        return kUnknownError;

    // check file properties
    if (!isInitialized())
        return kNotInitializedError;

    iLengthInFrames = getLengthIntern ();

    return kNoError;
}

Error_t CAudioFileIf::getPosition( long long &iFrame )
{
    // check file status
    if (!isOpen())
        return kUnknownError;

    // check file properties
    if (!isInitialized())
        return kNotInitializedError;

    iFrame = getPositionIntern();

    return kNoError;
}

Error_t CAudioFileIf::getPosition( double &dTimeInS )
{
    long long iFrame;

    // check file status
    if (!isOpen())
        return kUnknownError;

    // check file properties
    if (!isInitialized())
        return kNotInitializedError;

    dTimeInS = -1.;
    Error_t iErr = getPosition(iFrame);

    if (iErr != kNoError)
        return iErr;

    dTimeInS = iFrame * (1./m_sCurrFileSpec.fSampleRateInHz);
    return kNoError;
}

Error_t CAudioFileIf::getLength( double &dLengthInSeconds ) 
{
    long long iLengthInFrames;
    dLengthInSeconds = -1.;
    Error_t iErr = getLength (iLengthInFrames);

    if (iErr != kNoError)
        return iErr;

    dLengthInSeconds = iLengthInFrames * (1./m_sCurrFileSpec.fSampleRateInHz);
    return kNoError;
}

bool CAudioFileIf::isInitialized()
{
    return m_bIsInitialized;
}

Error_t CAudioFileIf::setInitialized( bool bInitialized /*= true*/ )
{
    m_bIsInitialized    = bInitialized;

    return kNoError;
}

Error_t CAudioFileIf::setIoType( FileIoType_t eIoType )
{
    m_eIoType   = eIoType;
    return kNoError;
}

CAudioFileIf::FileIoType_t CAudioFileIf::getIoType() const
{
    return m_eIoType;
}

Error_t CAudioFileIf::setFileSpec( const FileSpec_t *pFileSpec )
{
    memcpy (&m_sCurrFileSpec, pFileSpec, sizeof (FileSpec_t));
    return kNoError;
}

int CAudioFileIf::getNumChannels() const
{
    return m_sCurrFileSpec.iNumChannels;
}

Error_t CAudioFileIf::setNumBytesPerSample( int iNumBytes )
{
    assert (iNumBytes > 0);
    m_iNumBytesPerSample    = iNumBytes;
    return kNoError;
}

int CAudioFileIf::getNumBytesPerSample() const
{
    return m_iNumBytesPerSample;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

CAudioFileRaw::CAudioFileRaw(): CAudioFileIf() ,
    m_piTmpBuff(0)
{
    resetInstance (true);
}

CAudioFileRaw::~CAudioFileRaw()
{
    resetInstance (true);
}

Error_t CAudioFileRaw::freeMemory()
{
    CAudioFileIf::freeMemory ();
    if (!m_piTmpBuff)
        return kNoError;

    delete [] m_piTmpBuff;
    m_piTmpBuff  = 0;

    return kNoError;
}

Error_t CAudioFileRaw::allocMemory()
{
    freeMemory ();

    m_piTmpBuff = new short [m_kiDefBlockLength];

    if (!m_piTmpBuff)
        return kMemError;
    else
        return kNoError;
}

Error_t CAudioFileRaw::openFile( std::string cAudioFileName, FileIoType_t eIoType, FileSpec_t const *psFileSpec /*= 0*/ )
{
    if (cAudioFileName.empty())
        return kFileOpenError;

    resetInstance (true);
    setIoType(eIoType);

    // set file spec (required for raw streams)
    if (psFileSpec)
    {
        setFileSpec(psFileSpec);
        setInitialized(true);
    }
    // open file
    m_File.open (cAudioFileName.c_str(), ios::binary | ((eIoType & kFileRead)? ios::in : ios::out));

    if (!isOpen())
    {
        resetInstance (true);
        return kFileOpenError;
    }

    // allocate internal memory
    return allocMemory ();
}

Error_t CAudioFileRaw::closeFile()
{
    if (!isOpen())
    {
        return kNoError;
    }    

    m_File.close ();

    // free internal memory
    return freeMemory ();
}

bool CAudioFileRaw::isEof()
{
    return m_File.eof();
}

bool CAudioFileRaw::isOpen()
{
    return m_File.is_open ();
}

float CAudioFileRaw::scaleUp( float fSample2Clip )
{
    float fScale        = static_cast<float>(1<<(getNumBitsPerSample()-1));
    fSample2Clip *= fScale;
    if (isClippingEnabled())
    {
        fSample2Clip = clip2Range(fSample2Clip, -fScale, fScale-1);
    }
    return fSample2Clip;
}

float CAudioFileRaw::scaleDown( float fSample2Scale )
{
    float fScale        = static_cast<float>(1<<(getNumBitsPerSample()-1));
    return fSample2Scale/fScale;
}

int CAudioFileRaw::readDataIntern( float **ppfAudioData, int iLength )
{
    int iNumChannels    = getNumChannels();
    int iNumFrames2Read = min (iLength, m_kiDefBlockLength/iNumChannels);
    int iNumFrames      = 0;

    // sanity check
    assert (ppfAudioData || ppfAudioData[0]);

    // ugly hack
    // a) only for 16 bit input
    // b) only for little endian
    while (iNumFrames2Read > 0)
    {
        int iCurrFrames = iNumFrames2Read;
        m_File.read (reinterpret_cast<char*>(m_piTmpBuff), convFrames2Bytes(iNumFrames2Read));

        iNumFrames2Read = min (iLength-iCurrFrames, m_kiDefBlockLength/iNumChannels);

        if (!m_File)
        {
            iCurrFrames     = static_cast<int>(convBytes2Frames(m_File.gcount ()));
            iNumFrames2Read = 0;
        }

        // copy the data
        for (int iCh = 0; iCh < iNumChannels; iCh++)
        {
            for (int i = 0; i < iCurrFrames; i++)
            {
                ppfAudioData[iCh][iNumFrames+i] = scaleDown(static_cast<float> (m_piTmpBuff[i*iNumChannels+iCh]));
            }

        }
        // update frame counters
        iLength        -= iCurrFrames;
        iNumFrames     += iCurrFrames;

        assert (iLength >= 0);
    }

    // update iLength to the number of frames actually read
    return iNumFrames;

}

int CAudioFileRaw::writeDataIntern( float **ppfAudioData, int iLength )
{
    int iIdx            = 0;
    int iNumChannels    = getNumChannels();

    // sanity check
    assert (ppfAudioData || ppfAudioData[0]);

    // very ugly hack
    // a) only for 16 bit output
    // b) disregarded endianess
    while (iIdx < iLength)
    {
        int iNumFrames2Write = min (iLength-iIdx, m_kiDefBlockLength/iNumChannels);

        // copy the data
        for (int iCh = 0; iCh < iNumChannels; iCh++)
        {
            for (int i = 0; i < iNumFrames2Write; i++)
            {
                m_piTmpBuff[i*iNumChannels+iCh] = CUtil::float2int<short>(scaleUp(ppfAudioData[iCh][iIdx+i]));
            }
        }

        m_File.write (reinterpret_cast<char*>(m_piTmpBuff), convFrames2Bytes(iNumFrames2Write));

        if (!m_File)
        {
            break;
        }

        // update frame counter
        iIdx   += iNumFrames2Write;
    }
    return iIdx;
}

long long CAudioFileRaw::getLengthIntern() 
{
    assert(m_File);

    //static const int    iNumOfBytesPerSample = 2;
    long long iCurrPos  = getPositionIntern();
    long long iLength   = 0;

    m_File.seekg (0, m_File.end);

    iLength = convBytes2Frames(static_cast<long long>(m_File.tellg()));

    setPositionIntern(iCurrPos);

    return iLength;
}

long long CAudioFileRaw::getPositionIntern()
{
    assert(m_File);
    return convBytes2Frames(static_cast<long long>(m_File.tellg()));
}

Error_t CAudioFileRaw::setPositionIntern( long long iFrame )
{
    assert(m_File);
    assert(iFrame >= 0);

    m_File.seekg (convFrames2Bytes(iFrame), m_File.beg);

    return kNoError;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#ifdef WITH_SNDLIB
CAudioFileSndLib::CAudioFileSndLib(): CAudioFileIf() ,
    m_FileHandle(-1),
    m_lFrameCnt(0),
    m_lFileLength(0),
    m_ppdTmpBuff(0)
{
    resetInstance (true);
}

CAudioFileSndLib::~CAudioFileSndLib()
{
    resetInstance (true);
}


Error_t CAudioFileSndLib::openFile( std::string cAudioFileName, FileIoType_t eIoType, FileSpec_t const *psFileSpec /*= 0*/ )
{
    FileSpec_t  sSpec; 
    int iSndLibFileFormat = MUS_RIFF;

    if (cAudioFileName.empty())
        return kFileOpenError;

    resetInstance (true);
    setIoType(eIoType);

    if (!psFileSpec)
    {
        getFileSpec(sSpec);
    }
    else
    {
        CUtil::copyBuff(&sSpec, psFileSpec, 1);
    }

    switch (sSpec.eFormat)
    {
    case kFileFormatWav:
        iSndLibFileFormat = MUS_RIFF;
        break;
    case kFileFormatRaw:
        iSndLibFileFormat = MUS_RAW;
        break;
    case kFileFormatAiff:
        iSndLibFileFormat = MUS_AIFF;
        break;
    default:
        return kFunctionInvalidArgsError;
    }

    // set file spec (required for raw streams)
    setFileSpec(&sSpec);

    if (getIoType() == kFileRead)
    {
        if (mus_sound_header_type(cAudioFileName.c_str()) == MUS_RAW)
        {
            mus_sound_set_chans(cAudioFileName.c_str(), sSpec.iNumChannels);
            mus_sound_set_srate(cAudioFileName.c_str(), CUtil::float2int<int>(sSpec.fSampleRateInHz));
            mus_sound_set_header_type(cAudioFileName.c_str(),  iSndLibFileFormat);
            mus_sound_set_data_format(cAudioFileName.c_str(), (sSpec.eBitStreamType == kFileBitStreamInt16)?MUS_LSHORT:MUS_LFLOAT);
        }
        else
        {
            FileSpec_t  sFileSpec;
            int iTmp = -1;
            sFileSpec.fSampleRateInHz = static_cast<float>(mus_sound_srate(cAudioFileName.c_str()));
            sFileSpec.iNumChannels    = mus_sound_chans(cAudioFileName.c_str());
            iTmp    = mus_sound_header_type(cAudioFileName.c_str());
            switch (iTmp)
            {
            case MUS_RIFF:
                sFileSpec.eFormat = kFileFormatWav;
                break;
            case MUS_AIFF:
                sFileSpec.eFormat = kFileFormatAiff;
                break;
            default:
                sFileSpec.eFormat = kFileFormatUnknown;
                break;
            }
            iTmp    = mus_sound_data_format(cAudioFileName.c_str());
            switch (iTmp)
            {
            case MUS_LSHORT:
            case MUS_BSHORT:
                sFileSpec.eBitStreamType = kFileBitStreamInt16;
                setNumBytesPerSample(2);
                break;
            case MUS_BFLOAT:
            case MUS_LFLOAT:
                sFileSpec.eBitStreamType = kFileBitStreamFloat32;
                setNumBytesPerSample(4);
                break;
            default:
                sFileSpec.eBitStreamType = kFileBitStreamUnknown;
                break;
            }
            setFileSpec(&sFileSpec);
        }

        m_FileHandle = mus_sound_open_input(cAudioFileName.c_str());
    }
    else
    {
        m_FileHandle = mus_sound_open_output(cAudioFileName.c_str(), 
            CUtil::float2int<int>(sSpec.fSampleRateInHz),
            sSpec.iNumChannels,
            (sSpec.eBitStreamType == kFileBitStreamInt16)?MUS_LSHORT:MUS_LFLOAT, // only write little endian files
            iSndLibFileFormat,
            0);
    }

    if (!isOpen())
    {
        resetInstance (true);
        return kFileOpenError;
    }

    m_lFileLength = mus_sound_frames(cAudioFileName.c_str());
    setInitialized(true);

    // allocate internal memory
    return allocMemory ();
}

Error_t CAudioFileSndLib::closeFile()
{
    if (!isOpen())
    {
        return kNoError;
    }    

    if (getIoType() == kFileRead)
    {
        mus_sound_close_input(m_FileHandle);
    }
    else
    {        
        mus_sound_close_output(m_FileHandle, convFrames2Bytes(m_lFrameCnt));
    }

    m_lFrameCnt     = 0;
    m_FileHandle    = -1;
    m_lFileLength   = 0;

    // free internal memory
    return freeMemory ();
}

bool CAudioFileSndLib::isOpen()
{
    return (m_FileHandle >= 0);
}

int CAudioFileSndLib::readDataIntern( float **ppfAudioData, int iLength )
{
    int iNumChannels    = getNumChannels();
    int iNumFrames2Read = min (iLength, m_kiDefBlockLength);
    int iNumFrames      = 0;

    // sanity check
    assert (ppfAudioData || ppfAudioData[0]);

    // use internal buffer with fixed length
    while (iNumFrames2Read > 0)
    {
        int iCurrFrames = mus_sound_read(m_FileHandle, 0, iNumFrames2Read-1, getNumChannels(), m_ppdTmpBuff);

        if (iCurrFrames <= m_lFileLength-m_lFrameCnt)
        {
            iNumFrames2Read = min (iLength-iCurrFrames, m_kiDefBlockLength);
        }
        else
        {
            iCurrFrames     = static_cast<int>(m_lFileLength-m_lFrameCnt);
            iNumFrames2Read = 0;
        } 

        // copy the data
        for (int iCh = 0; iCh < iNumChannels; iCh++)
        {
            for (int i = 0; i < iCurrFrames; i++)
            {
                ppfAudioData[iCh][iNumFrames+i] = static_cast<float>(m_ppdTmpBuff[iCh][i]);
            }

        }
        // update frame counters
        iLength        -= iCurrFrames;
        iNumFrames     += iCurrFrames;
        m_lFrameCnt    += iCurrFrames;

        assert (iLength >= 0);
    }

    // update iLength to the number of frames actually read
    return iNumFrames;
}

int CAudioFileSndLib::writeDataIntern( float **ppfAudioData, int iLength )
{
    int iIdx            = 0;
    int iNumChannels    = getNumChannels();

    // sanity check
    assert (ppfAudioData || ppfAudioData[0]);
 
    float afRange[2] = {std::numeric_limits<float>::min(), std::numeric_limits<float>::max()};
    if (isClippingEnabled())
    {
        afRange[0]  = -1.F;
        afRange[1]  = 1-1.F/static_cast<float>(1<<(getNumBitsPerSample()));
    }

    // use internal buffer with fixed length
    while (iIdx < iLength)
    {
        int iNumFrames2Write = min (iLength-iIdx, m_kiDefBlockLength);

        // copy the data
        for (int iCh = 0; iCh < iNumChannels; iCh++)
        {
            for (int i = 0; i < iNumFrames2Write; i++)
            {
                m_ppdTmpBuff[iCh][i]    = clip2Range(ppfAudioData[iCh][iIdx+i], afRange[0], afRange[1]);
            }
        }

        if (mus_sound_write(m_FileHandle, 0, iNumFrames2Write-1, getNumChannels(), m_ppdTmpBuff) == 0)
        {
            // update frame counter
            iIdx        += iNumFrames2Write;
            m_lFrameCnt += iNumFrames2Write;
        }
    }
    
    return iIdx;
}

long long CAudioFileSndLib::getLengthIntern()
{
    if (getIoType() == kFileWrite)
    {
        return m_lFrameCnt;
    }
    else
    {
        return m_lFileLength;
    }
}

long long CAudioFileSndLib::getPositionIntern()
{
    return m_lFrameCnt;
}

Error_t CAudioFileSndLib::setPositionIntern( long long iFrame )
{
    m_lFrameCnt = convBytes2Frames(mus_sound_seek_frame(m_FileHandle, iFrame));
    
    return kNoError;
}

Error_t CAudioFileSndLib::freeMemory()
{
    CAudioFileIf::freeMemory ();
    if (!m_ppdTmpBuff)
        return kNoError;

    for (int i = 0; i < getNumChannels(); i++)
        delete m_ppdTmpBuff[i];

    delete [] m_ppdTmpBuff;
    m_ppdTmpBuff  = 0;

    return kNoError;
}

Error_t CAudioFileSndLib::allocMemory()
{
    int iNumChannels = getNumChannels();
    freeMemory ();

    m_ppdTmpBuff    = new double* [iNumChannels];
    for (int i = 0; i < iNumChannels; i++)
        m_ppdTmpBuff[i] = new double [m_kiDefBlockLength];

    if (!m_ppdTmpBuff)
        return kMemError;
    else
        return kNoError;
}

bool CAudioFileSndLib::isEof()
{
    return (m_lFrameCnt >= m_lFileLength);
}

#endif //WITH_SNDLIB
