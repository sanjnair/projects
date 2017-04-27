/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cexception.h"
#include "cifstream.h"
#include "cutil.h"

/*!
    Constructs Abstract stream
*/
CIfStream::CIfStream(CFile *f, bool binary)
 : CAbstractfStream(f), _binary(binary), _curPos(0), _fileSize(0) {}

CIfStream::~CIfStream() {}

/*!
  Opens the stream  
*/
void CIfStream::open()
{
    close();
    if (_binary) {
      _stream.open(_file->getName().c_str(), std::fstream::in | std::fstream::binary);
    } else {
      _stream.open(_file->getName().c_str(), std::fstream::in);
    }

    if (! _stream) {
      string msg ="Unable to open stream for reading ";
      msg += _file->getName();
      C_THROW(msg);
    }
}

/*!
  Closes the stream
*/
void CIfStream::close()
{
  CAbstractfStream::close();
  _curPos = 0;
  _fileSize = 0;  
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

/*!
  Reads at max maxLen chars from the stream.
*/
int CIfStream::read(char *buf, int maxLen)
{
  errorIfNotOpened();

  _stream.read(buf, maxLen);
  if(_stream.bad()) {
    string msg = "Unable to read from stream";
    C_THROW(msg);
  }
  
  return _stream.gcount();
}

