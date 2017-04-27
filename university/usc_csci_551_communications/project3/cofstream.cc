#include "cexception.h"
#include "cofstream.h"

/*!
    Constructs Abstract stream
*/
COfStream::COfStream(CFile *f, bool append)
  : CAbstractfStream(f)
{
  _append = append;
}

/*!
  D'tor
*/
COfStream::~COfStream() {}

/*!
  Opens the stream  
*/
void COfStream::open()
{
  close();

  std::ios_base::openmode mode = std::ofstream::out | std::ofstream::binary;
  if (_append) {
    mode |= std::ofstream::app;
  }
  
  _stream.open(_file->getName().c_str(), mode);
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
