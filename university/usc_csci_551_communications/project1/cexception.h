/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CEXCEPTION_H
#define CEXCEPTION_H

#include "cglobal.h"
#define C_THROW(msg) throw CException(msg, __FILE__, __LINE__);

class CException 
{
public:
    CException();
    CException(const string &m, const string f, int l);
    ~CException();

    string getMessage() const;
    string getFile() const;
    int getLine() const;
    string toString() const;

private:
    string _msg;
    string _file;
    int _line;
};

#endif


