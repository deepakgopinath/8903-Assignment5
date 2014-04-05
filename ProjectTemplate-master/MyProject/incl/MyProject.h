#if !defined(__MyProject_hdr__)
#define __MyProject_hdr__

#include "ErrorDef.h"

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
    
    Error_t initInstance (/*enter parameters here*/);
    Error_t resetInstance ();
    
    //virtual Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) = 0;

protected:
    CMyProject ();
    virtual ~CMyProject ();
};

#endif // #if !defined(__MyProject_hdr__)



