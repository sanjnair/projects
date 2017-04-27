/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CABSTRACTFSTREAM_H
#define CABSTRACTFSTREAM_H

#define FILE_BUF_SIZE 4096

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

    void flush();

    CFile *getFile() const;

protected:
    void errorIfNotOpened();

    CFile *_file;
    std::fstream _stream;
};

#endif //CABSTRACTFSTREAM_H

