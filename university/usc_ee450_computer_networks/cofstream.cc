#include "cexception.h"
#include "cofstream.h"

/*!
    Constructs Abstract stream
*/
COfStream::COfStream(CFile *f) : CAbstractfStream(f) {}

COfStream::~COfStream() {}


/*!
  Opens the stream  
*/
void COfStream::open()
{
    close();
    _stream.open(_file->getName().c_str(), std::ifstream::out);
}


/*!
  Opens the stream  with the given open mode
*/
void COfStream::open(std::ios_base::openmode mode)
{
    close();
    _stream.open(_file->getName().c_str(), mode);
}

/*!
    Writes to the stream
*/
void COfStream::write(const char *buf, int size)
{
    errorIfNotOpened();
    _stream.write(buf, size);

}

/*!
    \overload
    Overload method provided for convenience.
*/
void COfStream::write(const string &s)
{
    write(s.c_str(), s.length());
}
