
#include <iostream>
#include <vector>
#include "MyProjectConfig.h"

// include project headers
#include "AudioFileIf.h"

#include "MyProject.h"

#define WITH_FLOATEXCEPTIONS
#define WITH_MEMORYCHECK

// include exception header
#if (defined(WITH_FLOATEXCEPTIONS) && !defined(NDEBUG) && defined (GTCMT_WIN32))
#include <float.h>
#endif // #ifndef WITHOUT_EXCEPTIONS

// include memory leak header
#if (defined(WITH_MEMORYCHECK) && !defined(NDEBUG) && defined (GTCMT_WIN32))
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

Error_t   getClArgs (std::string &sInputFilePath, std::string &sOutputFilePath, int &blockSize, int &hopSize, int argc, char* argv[]);

void    printVector(std::vector<int> vector);
/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,
                            sOutputFilePath;

    float                   **ppfAudioData  = 0;
    float                   **ppfFeatureMatrix = 0;
    static const int        kBlockSize      = 1024;

    
    CAudioFileIf            *phInputFile    = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;
    CMyProject              *pMyFeatureExtractor;
    std::vector<int>        myOptionsArray;
    int                     userEnteredOption = 0;
    int                     iBlockSize = 1024;
    int                     iHopSize = 512;
    int                     iNumBlocks = 1;
    int                     xDim = 0;
    int                     yDim = 0;
    long long               iNumFramesInAudioFile = 0;
    
    CFft::WindowFunction_t windowFunction = CFft::kWindowHamming;
    Error_t err;

    // detect memory leaks in win32
#if (defined(WITH_MEMORYCHECK) && !defined(NDEBUG) && defined (GTCMT_WIN32))
    // set memory checking flags
    int iDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    iDbgFlag       |= _CRTDBG_CHECK_ALWAYS_DF;
    iDbgFlag       |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag( iDbgFlag );
#endif

    // enable floating point exceptions in win32
#if (defined(WITH_FLOATEXCEPTIONS) && !defined(NDEBUG) && defined (GTCMT_WIN32))
    // enable check for exceptions (don't forget to enable stop in MSVC!)
    _controlfp(~(_EM_INVALID | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_DENORMAL), _MCW_EM) ;
#endif // #ifndef WITHOUT_EXCEPTIONS

    showClInfo ();

    // parse command line arguments
    err = getClArgs(sInputFilePath, sOutputFilePath, iBlockSize, iHopSize, argc, argv);
    
    if(err != kNoError)
    {
        return -1;
    }
    // open the input wave file
    CAudioFileIf::createInstance(phInputFile);
    phInputFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phInputFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phInputFile->getFileSpec(stFileSpec);
    phInputFile->getLength(iNumFramesInAudioFile);
   // std::cout<<iNumFramesInAudioFile<<std::endl;
    iNumBlocks = static_cast<int>(floorf((iNumFramesInAudioFile - iBlockSize)/iHopSize)) + 1;
   // std::cout << iNumBlocks << std::endl;
    // open the output text file
     hOutputFile.open (sOutputFilePath.c_str(), std::ios::out);
     if (!hOutputFile.is_open())
     {
         cout << "Text file open error!";
         return -1;
     }
    
    err = CMyProject::createInstance(pMyFeatureExtractor);
    if(err != kNoError)
    {
        std::cout << "Error instantiating the feature extractor" << std::endl;
        return -1;
    }
    
    std::cout << "Please enter your feature selections" << std::endl << "0: Spectral Centroid" << std::endl <<"1: Spectral Flux" << std::endl << "2: Spectral Rolloff" << std::endl << "3: Zero Cross Rating" << std::endl << "(Enter -1 to stop entering options)";
    
    while(userEnteredOption != -1)
    {
        std::cin >> userEnteredOption;
        std::cout<< "Here is what you entered" << std::endl << userEnteredOption;
        if(userEnteredOption != -1)
        {
            myOptionsArray.push_back(userEnteredOption);
        }
        std::cin.clear();
        std::cin.ignore();
    }
    
   // printVector(myOptionsArray);
    // print the feature option list user
    
    /*
        Select your features
        0 : spectral Centroid
        1 : spectral flux
        2 : spectral rolloff
        3 : zero crossing
     */
    
    
    
    // what do we know here
    
    /*
        sampling rate from audio file
        num channels from audio file
        block size from user
        hopsize from user
        window function type from user
        we should also have constructed our options array properly.
     */
    
    pMyFeatureExtractor->initInstance(stFileSpec.iNumChannels, stFileSpec.fSampleRateInHz, iBlockSize, iHopSize, windowFunction, myOptionsArray, iNumBlocks);
    
    // we call initInstance of pMyProject
    // allocate audio data buffer
    ppfAudioData            = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float [kBlockSize];

    // read wave
    while (!phInputFile->isEof())
    {
        int iNumFrames = kBlockSize;
        phInputFile->readData(ppfAudioData, iNumFrames);
        pMyFeatureExtractor->process(ppfAudioData, ppfAudioData, iNumFrames);
//        for (int i = 0; i < iNumFrames; i++)
//        {
//            for (int c = 0; c < stFileSpec.iNumChannels; c++)
//            {
//                hOutputFile << ppfAudioData[c][i] << "\t";
//            }
//            hOutputFile << endl;
//        }

    }
    
    // here the feature matrix should have been populated....
    pMyFeatureExtractor->getSizeOfResult(xDim, yDim);
    std::cout << " Size of feature Matrix is " << xDim << " by " << yDim;
    
    pMyFeatureExtractor->getResult(ppfFeatureMatrix);
    
    std::cout << "Write the features onto the command line" << std::endl;
    
    for (int i=0; i<xDim; i++)
    {
        for(int j=0; j<yDim; j++)
        {
            std::cout << ppfFeatureMatrix[i][j] << "\t";
            hOutputFile << ppfFeatureMatrix[i][j] << "\t";
        }
        std::cout << std::endl;
        hOutputFile << endl;
    }
    
    
    // close the files
    CAudioFileIf::destroyInstance(phInputFile);
    hOutputFile.close();
    
    CMyProject::destroyInstance(pMyFeatureExtractor);
    // free memory
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete [] ppfAudioData[i];
    delete [] ppfAudioData;
    ppfAudioData = 0;

    return 0;
    
}


void     showClInfo()
{
    cout << "GTCMT template app" << endl;
    cout << "(c) 2013 by Alexander Lerch" << endl;
    cout    << "V" 
        << CMyProject::getVersion (CMyProject::kMajor) << "." 
        << CMyProject::getVersion (CMyProject::kMinor) << "." 
        << CMyProject::getVersion (CMyProject::kPatch) << ", date: " 
        << CMyProject::getBuildDate () << endl;
    cout  << endl;

    return;
}

Error_t getClArgs( std::string &sInputFilePath, std::string &sOutputFilePath, int &blockSize, int &hopSize,int argc, char* argv[])
{
    if (argc > 1)
        sInputFilePath.assign (argv[1]);
    if (argc > 2)
        sOutputFilePath.assign (argv[2]);
    if(argc > 3) // iBlockSize;
    {
        if(atof(argv[3]) > 1.0 && CUtil::isPowOf2(static_cast<int>(atof(argv[3]))))
        {
            blockSize = static_cast<int>(atof(argv[3]));
        }
        else
        {
            std::cout << "Invalid block size. Gotta be a power of 2 baggaaa" << std::endl;
            return kFunctionInvalidArgsError;
        }
    }
    if(argc > 4)
    {
        if(atof(argv[4]) > 1.0 && static_cast<int>(atof(argv[4])) < blockSize)
        {
            hopSize = static_cast<int>(atof(argv[4]));
        }
        else
        {
            std::cout << "Invalid hopsize" << std::endl;
            return kFunctionInvalidArgsError;
        }
    }
    return kNoError;
}

void printVector(std::vector<int>vec)
{
    for(int i=0; i<vec.size(); i++)
    {
        std::cout << vec[i] << std::endl;
    }
    
}