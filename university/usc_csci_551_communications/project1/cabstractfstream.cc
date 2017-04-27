/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cexception.h"
#include "cabstractfstream.h"

/*!
    Constructs Abstract stream
*/
CAbstractfStream::CAbstractfStream(CFile *f) : _file(f){}

/*!
    Deletes Abstract stream
*/
CAbstractfStream::~CAbstractfStream() {}

/*!
    Returns underlying file
*/
CFile* CAbstractfStream::getFile() const
{
    return _file;
}

/*!
    Return true of the stream is opened
 */
bool CAbstractfStream::isOpened()
{
    return static_cast<bool>(_stream.is_open());
}

/*!
    Return true of the stream is opened
 */
void CAbstractfStream::close()
{
    if (_stream.is_open()) {
        _stream.close();
    }
}

 /*!
    Returns true if EOF reached
*/
bool CAbstractfStream::isEof()
 {
     errorIfNotOpened();
     return static_cast<bool>(_stream.eof());
 }

 /*!
    Returns true if the stream is good
*/
bool CAbstractfStream::isGood()
 {
    errorIfNotOpened();
    return static_cast<bool>(_stream.good());
 }

/*!
    Throws exception if the file is not opened
*/
void CAbstractfStream::errorIfNotOpened()
{
    if (! isOpened()) {
        string msg("File ");
        msg += _file->getName();
        msg += " is not opened";
        C_THROW(msg);
    }
}
