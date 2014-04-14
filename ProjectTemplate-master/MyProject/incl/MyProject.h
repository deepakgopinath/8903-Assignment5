#if !defined(__MyProject_hdr__)
#define __MyProject_hdr__

#include "ErrorDef.h"
#include <vector.h>
#include "Dsp.h"
#include "InputBuffSrc.h"
#include "CFeatureExtractor.h"

class CMyProject
{
public:
    /*! version number */
    enum Version_t
    {
        kMajor,                         //!< major version number
        kMinor,                         //!< minor version number
        kPatch,                         //!< patch version number

        kNumVersionInts
    };
    
    
    static const int  getVersion (const Version_t eVersionIdx);
    static const char* getBuildDate ();

    static Error_t createInstance (CMyProject*& pCKortIf);
    static Error_t destroyInstance (CMyProject*& pCKortIf);
    
    Error_t initInstance (int iNumChannels, float fSampleRate, int iBlockSize, int iHopSize, CFft::WindowFunction_t eWindowFunction, CFft::Windowing_t eWindow, float fKappa, std::vector<int>optionsArray, int iNumBlocks);
    
    
    Error_t resetInstance ();
    
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    
    void getSizeOfResult(int &numRows, int &numColumns);
    void getResult(float **&featureMatrix);
protected:
    CMyProject ();
    virtual ~CMyProject ();
    
private:
    
    bool m_bIsInitialized;
    bool m_IsSpectralFeatureRequested;
    int m_iNumChannels;
    float m_fSampleRate;
    int m_iBlockSize;
    int m_iHopSize;
    int m_iNumBlocks;
    int m_iBlockCounter;
    CFft::complex_t *m_FFTResult;
    float *m_fFftMag;
    std::vector<int>m_vOptionsArray;
    
    float **m_ProcBuff;
    float *m_DownMixedAudio;
    
    float **m_FeatureMatrix;
    CFft *m_pMyFFt;
    CInputBuffSrc<float> *m_pMyInputBuffSrc;
    CFeatureExtractor *m_pFeatureExtractor;
    
    
    
};

#endif // #if !defined(__MyProject_hdr__)



