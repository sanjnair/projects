/**
 * Abstract Stream
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __COFSTREAM_H__
#define __COFSTREAM_H__

#include "cabstractfstream.h"
#include <iostream>

class COfStream : public CAbstractfStream {

public:
    COfStream(CFile *f);
    virtual ~COfStream();
    
    void open();
    void open(std::ios_base::openmode mode);
    void write(const char *buf, int size);
    void write(const string &s);

private:
    
};

#endif //__COFSTREAM_H__

