#if !defined(__AudioFileIf_hdr__)
#define __AudioFileIf_hdr__

#include <string>
#include <fstream>
#include "ErrorDef.h"

/*! \class open, read, and write audio files
*/
class CAudioFileIf
{
public:
    enum FileIoType_t
    {
        kFileRead   = 0x1L,     //!< open file for reading
        kFileWrite  = 0x2L,     //!< open file for writing

        kNumFileOpenTypes = 2
    };
    enum FileFormat_t
    {
        kFileFormatRaw,         //!< file is raw pcm format
        kFileFormatWav,         //!< file is wav riff format (not available without sndlib)
        kFileFormatAiff,        //!< file is aiff format (not available without sndlib)
        kFileFormatUnknown,     //!< file format is unknown

        kNumFileFormats
    };
    enum BitStream_t
    {
        kFileBitStreamInt16,    //!< a sample is two byte (int16)
        kFileBitStreamFloat32,  //!< a sample is four byte (float32)  (not available without sndlib)
        kFileBitStreamUnknown,  //!< word length is unknown

        kNumWordLengths
    };
    struct FileSpec_t
    {
        FileFormat_t    eFormat;            //!< file format (wav, aiff, raw)
        BitStream_t     eBitStreamType;     //!< word length and sample type
        int             iNumChannels;       //!< number of audio channels
        float           fSampleRateInHz;    //!< sample rate in Hz
    };

    /*! opens a new instance for audio file IO
    \param CAudioFileIf * & pCInstance
    \return Error_t
    */
    static Error_t createInstance (CAudioFileIf*& pCInstance);
    /*! destroys and audio file IO instance
    \param CAudioFileIf * & pCInstance
    \return Error_t
    */
    static Error_t destroyInstance (CAudioFileIf*& pCInstance);
    /*! reset instance to initial state
    \param bool bFreeMemory: also free the internal memory if true
    \return Error_t
    */
    virtual Error_t resetInstance (bool bFreeMemory = false);

    /*! open a file for reading or writing
    \param std::string cAudioFileName
    \param FileIoType_t eIoType
    \param FileSpec_t const * psFileSpec
    \return Error_t
    */
    virtual Error_t openFile (std::string cAudioFileName, FileIoType_t eIoType, FileSpec_t const *psFileSpec = 0) = 0;
    /*! close the current file
    \return Error_t
    */
    virtual Error_t closeFile () = 0;

    /*! read data from file
    \param float * * ppfAudioData: [channels][iNumFrames]
    \param int & iNumFrames: number of frames to read (per channel)
    \return Error_t
    */
    virtual Error_t readData (float **ppfAudioData, int &iNumFrames);
    /*! write data to file
    \param float * * ppfAudioData: [channels][iNumFrames]
    \param int iNumFrames: number of frames to write (per channel)
    \return Error_t
    */
    virtual Error_t writeData (float **ppfAudioData, int iNumFrames);

    /*! retrieve file specifications
    \param FileSpec_t & sFileSpec
    \return Error_t
    */
    Error_t getFileSpec (FileSpec_t &sFileSpec);

    /*! jump to new position in file
    \param long long iFrame: frame to jump to
    \return Error_t
    */
    virtual Error_t setPosition (long long iFrame = 0);
    /*! jump to new position in file
    \param double dTimeInS: time to jump to
    \return Error_t
    */
    Error_t setPosition (double dTimeInS = .0);

    /*! enable clipping to avoid wrap-arounds
    \param bool bIsEnabled
    \return Error_t
    */
    Error_t setClippingEnabled (bool bIsEnabled = true);
    /*! check if clipping is enabled
    \return bool
    */
    bool isClippingEnabled () {return m_bWithClipping;};

    /*! get current position in file
    \param long long & iFrame: current frame
    \return Error_t
    */
    Error_t getPosition (long long &iFrame);
    /*! get current position in file
    \param double & dTimeInS: current time in seconds
    \return Error_t
    */
    Error_t getPosition (double &dTimeInS);
    /*! get length of file
    \param long long & iLengthInFrames: file length in frames
    \return Error_t
    */
    Error_t getLength (long long &iLengthInFrames) ;
    /*! get length of file
    \param double & dLengthInSeconds: file length in seconds
    \return Error_t
    */
    Error_t getLength (double &dLengthInSeconds) ;

    /*! check if EOF is true
    \return bool
    */
    virtual bool isEof () = 0;
    /*! check if a file is opened
    \return bool
    */
    virtual bool isOpen () = 0;
    /*! check is the instance is initialized
    \return bool
    */
    virtual bool isInitialized ();

protected:

    CAudioFileIf ();
    virtual ~CAudioFileIf ();
    virtual Error_t freeMemory ();
    virtual Error_t allocMemory ();

    Error_t setInitialized (bool bInitialized = true);
    Error_t setIoType (FileIoType_t eIoType);
    FileIoType_t getIoType () const;
    Error_t setFileSpec (const FileSpec_t *pFileSpec);
    int getNumChannels () const;
    long long convFrames2Bytes (long long iNumFrames);
    long long convBytes2Frames (long long iNumFrames);
    Error_t setNumBytesPerSample (int iNumBytes);
    int getNumBytesPerSample () const;
    int getNumBitsPerSample () const {return (m_iNumBytesPerSample<<3);}

    float clip2Range(float fSample2Clip, float fMin, float fMax)
    {
        fSample2Clip = std::min (fSample2Clip, fMax);
        fSample2Clip = std::max (fSample2Clip, fMin);
        return fSample2Clip;
    }

    static const int    m_kiDefBlockLength;     //!< buffer length for read and write operations

private:
    virtual Error_t initDefaults ();

    virtual int readDataIntern (float **ppfAudioData, int iNumFrames) = 0;
    virtual int writeDataIntern (float **ppfAudioData, int iNumFrames) = 0;
    virtual long long getLengthIntern() = 0;
    virtual long long getPositionIntern() = 0;
    virtual Error_t setPositionIntern( long long iFrame ) = 0;

    FileSpec_t      m_sCurrFileSpec;            //!< current file specifications
    FileIoType_t    m_eIoType;                  //!< read or write

    bool            m_bWithClipping;            //!< true if abs(values ) > 1 should be clipped
    bool            m_bIsInitialized;           //!< true if initialized
    int             m_iNumBytesPerSample;       //!< number of bytes per sample for the raw pcm IO option without sndlib

};


/*! \class open, read, and write raw audio files in 16 bit integer little endian format.
*/
class CAudioFileRaw : public CAudioFileIf
{
public:
    CAudioFileRaw ();
    virtual ~CAudioFileRaw ();

    Error_t openFile (std::string cAudioFileName, FileIoType_t eIoType, FileSpec_t const *psFileSpec = 0);
    Error_t closeFile ();
    bool isEof ();
    bool isOpen ();

private:
    Error_t freeMemory ();
    Error_t allocMemory ();
    int readDataIntern (float **ppfAudioData, int iNumFrames);
    int writeDataIntern (float **ppfAudioData, int iNumFrames);
    long long getLengthIntern();
    long long getPositionIntern();
    Error_t setPositionIntern( long long iFrame );

    float scaleUp (float fSample2Clip);
    float scaleDown (float fSample2Scale);

    std::fstream        m_File;                 //!< raw pcm file handle
    short               *m_piTmpBuff;           //!< temporary buffer for 16 bit integer values
};


/*! \class open, read, and write audio files with sndlib (CMake option WITH_SNDLIB has to be ON)
*/
class CAudioFileSndLib : public CAudioFileIf
{
public:
    CAudioFileSndLib ();
    virtual ~CAudioFileSndLib ();

    Error_t openFile (std::string cAudioFileName, FileIoType_t eIoType, FileSpec_t const *psFileSpec = 0);
    Error_t closeFile ();
    bool isEof ();
    bool isOpen ();
    Error_t freeMemory ();
    Error_t allocMemory ();

private:
    int readDataIntern (float **ppfAudioData, int iNumFrames);
    int writeDataIntern (float **ppfAudioData, int iNumFrames);
    long long getLengthIntern();
    long long getPositionIntern();
    Error_t setPositionIntern( long long iFrame );

    int m_FileHandle;                           //!< sndlib file handle

    long long m_lFrameCnt;                      //!< current file position in frames
    long long m_lFileLength;                    //!< file length in frames
    double    **m_ppdTmpBuff;                   //!< temporary buffer for double values

};
#endif // #if !defined(__AudioFileIf_hdr__)
