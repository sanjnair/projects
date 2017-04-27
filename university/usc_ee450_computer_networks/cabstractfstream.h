/**
 * Abstract Stream
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CABSTRACTFSTREAM_H__
#define __CABSTRACTFSTREAM_H__

#define FILE_BUF_SIZE 1024

#include <fstream>
#include "cfile.h"

class CAbstractfStream {

public:
    CAbstractfStream(CFile *f);
    virtual ~CAbstractfStream();
    
    virtual void open() = 0;
    virtual void close();
    bool isOpened();
    bool isEof();
    bool isGood();

    CFile *getFile() const;
    
protected:
    void errorIfNotOpened();

    CFile *_file;
    std::fstream _stream;
};

#endif //__CABSTRACTFSTREAM_H__

