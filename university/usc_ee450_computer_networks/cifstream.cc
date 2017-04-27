#include "cexception.h"
#include "cifstream.h"

/*!
    Constructs Abstract stream
*/
CIfStream::CIfStream(CFile *f) : CAbstractfStream(f) {}

CIfStream::~CIfStream() {}

/*!
  Opens the stream  
*/
void CIfStream::open()
{
    close();
    _stream.open(_file->getName().c_str(), std::ifstream::in);
}

/*!
    Reads a line from the input stream
*/
string CIfStream::readLine()
{
    errorIfNotOpened();

    char buf[FILE_BUF_SIZE + 1];
    _stream.getline(buf, FILE_BUF_SIZE);

    return string(buf);
}

