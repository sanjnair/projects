/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/
#include "cmd5data.h"
#include "cutil.h"
#include "cfile.h"
#include "cifstream.h"

/**
 * C'tor.
 */
CMd5Data::CMd5Data()
{
    _initialized = false;
}

/**
 * Virtual D'tor.
 */
CMd5Data::~CMd5Data() {}

/**
 * Calculates SHA-1 on the file pointed by fp
 *
 * @param fp                    File pointer
 */
void CMd5Data::calculate(const string &fPath)
{
  CFile::errorIfNotExist(fPath);
  CFile f(fPath);
  CIfStream stream(&f, true);
  stream.open();

  UCharBuffer buf;
  buf.resize(BUF_SIZE);
  int readBytes = 0;

  try {
    while (stream.isGood() && (! stream.isEof())) {
      readBytes = stream.read(reinterpret_cast<char*>(&(buf[0])), BUF_SIZE);
      update(&(buf[0]), readBytes);
    }
    final();

  } catch (CException &e) {
    stream.close();
    throw e;
  }
}


/**
 * updates the md5 with the data
 *
 * @param buffer_ptr            Buffer
 * @param bufferLen             Buffer Len
 */
void CMd5Data::update(
  const uchar *buffer_ptr, ulong bufferLen)
{
  if(!_initialized) {
    MD5_Init(&_md5Ctx);
    _initialized = true;
  }
  MD5_Update(&_md5Ctx, buffer_ptr, bufferLen);
}

/**
 * updates the md5 with the data
 *
 * @param buffer_ptr            Buffer
 * @param bufferLen             Buffer Len
 */
void CMd5Data::final()
{
  if(_initialized) {
    _buffer.resize(MD5_DIGEST_LENGTH);
    MD5_Final(&(_buffer[0]), &_md5Ctx);
  } else {
    C_THROW("CMd5Data::final: Not initialized");
  }
}

/**
 * Returns the md5 buffer
 */
UCharBuffer CMd5Data::getData() const
{
    return _buffer;
}

/*!
  Gets the Md5 value for the given string
*/
UCharBuffer CMd5Data::getMd5(const string &s)
{
  UCharBuffer b;
  b.resize(MD5_DIGEST_LENGTH);

  MD5(reinterpret_cast<const uchar*>(s.c_str()), s.length(), &(b[0]));
  return b;
}


//-----------------------------------------------------------------------------
const uint CMd5Data::BUF_SIZE       = 4096;
