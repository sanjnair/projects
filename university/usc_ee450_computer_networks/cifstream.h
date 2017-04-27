/**
 * Abstract Stream
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CIFSTREAM_H__
#define __CIFSTREAM_H__

#include "cabstractfstream.h"

class CIfStream : public CAbstractfStream {

public:
    CIfStream(CFile *f);
    virtual ~CIfStream();
    
    void open();
    void eof() const;
    string readLine();

private:
    
};

#endif //__CIFSTREAM_H__

