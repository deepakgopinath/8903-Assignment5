
// standard headers

// project headers
#include "MyProjectConfig.h"

#include "ErrorDef.h"

#include "Util.h"
#include "MyProject.h"

static const char*  kCMyProjectBuildDate             = __DATE__;


CMyProject::CMyProject ():
            m_bIsInitialized(false),
            m_IsSpectralFeatureRequested(false),
            m_iNumChannels(0),
            m_fSampleRate(0),
            m_iBlockSize(0),
            m_iHopSize(0),
            m_iNumBlocks(0),
            m_iBlockCounter(0),
            m_FFTResult(0),
            m_ProcBuff(0),
            m_DownMixedAudio(0),
            m_FeatureMatrix(0),
            m_pMyFFt(0),
            m_pMyInputBuffSrc(0),
            m_pFeatureExtractor(0)
{
    // this never hurts
   
    
   // this->resetInstance ();
}


CMyProject::~CMyProject ()
{
    this->resetInstance ();
}

const int  CMyProject::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;

    switch (eVersionIdx)
    {
    case kMajor:
        iVersion    = MyProject_VERSION_MAJOR; 
        break;
    case kMinor:
        iVersion    = MyProject_VERSION_MINOR; 
        break;
    case kPatch:
        iVersion    = MyProject_VERSION_PATCH; 
        break;
    case kNumVersionInts:
        iVersion    = -1;
        break;
    }

    return iVersion;
}
const char*  CMyProject::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t CMyProject::createInstance (CMyProject*& pCMyProject)
{
    pCMyProject = new CMyProject ();
    
    if (!pCMyProject)
        return kUnknownError;


    return kNoError;
}

Error_t CMyProject::destroyInstance (CMyProject*& pCMyProject)
{
    if (!pCMyProject)
        return kUnknownError;
    
    pCMyProject->resetInstance ();
    
    delete pCMyProject;
    pCMyProject = 0;

    return kNoError;

}

Error_t CMyProject::initInstance(int iNumChannels, float fSampleRate, int iBlockSize, int iHopSize, CFft::WindowFunction_t eWindowFunctionType, CFft::Windowing_t eWindow, float fKappa, std::vector<int>optionsArray, int iNumBlocks)
{
    // allocate memory

    // initialize variables and buffers
    
    m_iNumChannels = iNumChannels;
    m_iBlockSize = iBlockSize;
    m_iHopSize = iHopSize;
    m_fSampleRate = fSampleRate;
    m_vOptionsArray = optionsArray;
    m_iNumBlocks = iNumBlocks;
    
    m_FFTResult = new float[m_iBlockSize];
    m_fFftMag = new float[static_cast<int>(m_iBlockSize/2) + 1];
    m_pMyInputBuffSrc  = new CInputBuffSrc<float>(m_iNumChannels, m_iBlockSize, 0);
    
    CFft::createInstance(m_pMyFFt);
    m_pMyFFt->initInstance(m_iBlockSize, 1, eWindowFunctionType, eWindow);
    
    
    m_pFeatureExtractor = new CFeatureExtractor(m_fSampleRate, static_cast<int>(m_iBlockSize/2) + 1, fKappa);
    m_ProcBuff = new float*[m_iNumChannels];
    m_FeatureMatrix = new float*[optionsArray.size()];
    m_DownMixedAudio =  new float[m_iBlockSize];
    
    for (int i=0; i < optionsArray.size(); i++) // check whether spectral feature was requested by the user
    {
        if (optionsArray[i] < CFeatureExtractor::kNumSpectralFeatures)
        {
            m_IsSpectralFeatureRequested =  true;
            break;
        }
    }
    
    for(int i=0; i<optionsArray.size(); i++)
    {
        m_FeatureMatrix[i] =  new float[m_iNumBlocks];
        CUtil::setZero(m_FeatureMatrix[i], m_iNumBlocks);
    }
    
    for(int i=0; i < m_iNumChannels; i++)
    {
        m_ProcBuff[i] =  new float[m_iBlockSize];
        CUtil::setZero(m_ProcBuff[i], m_iBlockSize);
    }
    m_bIsInitialized = true;
    return kNoError;
    
}

Error_t CMyProject::resetInstance ()
{
    // reset buffers and variables to default values
    
    if(m_bIsInitialized)
    {
        if(m_pMyFFt)
        {
            m_pMyFFt->resetInstance();
            m_pMyFFt->destroyInstance(m_pMyFFt);
        }
        if(m_pMyInputBuffSrc)
        {
            delete m_pMyInputBuffSrc;
        }
        if(m_pFeatureExtractor)
        {
            delete m_pFeatureExtractor;
        }
        if(m_FFTResult)
        {
            delete [] m_FFTResult;
        }
        if(m_fFftMag)
        {
            delete [] m_fFftMag;
        }
        if(m_ProcBuff)
        {
            for (int i = 0; i < m_iNumChannels; i++) {
                delete [] m_ProcBuff[i];
            }
            delete [] m_ProcBuff;
        }
        if (m_FeatureMatrix) {
            for (int i = 0; i < m_vOptionsArray.size(); i++) {
                delete [] m_FeatureMatrix[i];
            }
            delete [] m_FeatureMatrix;
        }
        if(m_DownMixedAudio)
        {
            delete [] m_DownMixedAudio;
        }
    }
    m_pMyFFt = 0;
    m_pMyInputBuffSrc = 0;
    m_pFeatureExtractor = 0;
    m_FFTResult = 0;
    m_fFftMag = 0;
    m_ProcBuff = 0;
    m_FeatureMatrix = 0;
    m_DownMixedAudio = 0;
    
    m_vOptionsArray.clear();
    
    
    m_IsSpectralFeatureRequested = false;
    m_iNumChannels = 0;
    m_iBlockSize = 0;
    m_iHopSize = 0;
    m_iNumBlocks = 0;
    m_iBlockCounter = 0;
    m_fSampleRate = 0.0;
    m_bIsInitialized = false;
    return kNoError;
}

Error_t CMyProject::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    m_pMyInputBuffSrc->setDataPtr2Hold(ppfInputBuffer, iNumberOfFrames);
    while(m_pMyInputBuffSrc->getBlock(m_ProcBuff, m_iBlockSize, m_iHopSize))
    {
        //downmixing
        CUtil::setZero(m_DownMixedAudio, m_iBlockSize); // clean up downmixed audio every block.
        for(int i=0; i<m_iBlockSize; i++)
        {
            for(int j=0; j<m_iNumChannels; j++)
            {
                m_DownMixedAudio[i] += m_ProcBuff[j][i];
            }
            m_DownMixedAudio[i] = static_cast<float>(m_DownMixedAudio[i]/m_iNumChannels);
        }
        
        if(m_IsSpectralFeatureRequested) // check for spectral, if yes, calculate fft.
        {
            m_pMyFFt->doFft(m_FFTResult, m_DownMixedAudio);
            m_pMyFFt->getMagnitude(m_fFftMag, m_FFTResult);
//            for(int i=0; i < (m_iBlockSize/2 + 1); i++)
//            {
//                std::cout<<m_fFftMag[i]<<std::endl;
//            }
        }
        
        for(int i=0; i < m_vOptionsArray.size(); i++)
        {
            float result = 0.0;
            if(m_vOptionsArray[i] < CFeatureExtractor::kNumSpectralFeatures)
            {
                result =  m_pFeatureExtractor->extractFeatures(m_fFftMag, static_cast<int>(m_iBlockSize/2) + 1, m_vOptionsArray[i]);
            }else // Time domain features
            {
                result = m_pFeatureExtractor->extractFeatures(m_DownMixedAudio, m_iBlockSize, m_vOptionsArray[i]);
            }
            
            m_FeatureMatrix[i][m_iBlockCounter] = result;
        }
        
        m_iBlockCounter++;
    }
    m_pMyInputBuffSrc->releaseDataPtr();

    return kNoError;
}

void CMyProject::getSizeOfResult(int &numRows, int &numColumns)
{
    numRows = m_vOptionsArray.size();
    numColumns =  m_iNumBlocks;
}

void CMyProject::getResult(float **&featureMatrix)
{
    featureMatrix = m_FeatureMatrix;
}

