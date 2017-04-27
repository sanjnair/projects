/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef COFSTREAM_H
#define COFSTREAM_H

#include <iostream>
#include "cabstractfstream.h"

class COfStream : public CAbstractfStream {

public:
    COfStream(CFile *f, bool append=false);
    virtual ~COfStream();

    void open();
    void open(std::ios_base::openmode mode);
    void write(const char *buf, int size);
    void write(const string &s);

private:
  bool _append;
};

#endif //COFSTREAM_H

