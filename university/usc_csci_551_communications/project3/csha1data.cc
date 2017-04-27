/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include "csha1data.h"
#include "cautoptr.h"
#include "cutil.h"
#include "cfile.h"
#include "cifstream.h"

/**
 * C'tor.
 */
CSha1Data::CSha1Data()
{
    _initialized = false;
}

/**
 * Virtual D'tor.
 */
CSha1Data::~CSha1Data() {}

/**
 * Calculates SHA-1 on the file pointed by fp
 *
 * @param fp                    File pointer
 */
void CSha1Data::calculate(const string &fPath)
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
 * updates the sha-1 with the data
 *
 * @param buffer_ptr            Buffer
 * @param bufferLen             Buffer Len
 */
void CSha1Data::update(
  const uchar *buffer_ptr, ulong bufferLen)
{
  if(!_initialized) {
    SHA1_Init(&_shaCtx);
    _initialized = true;
  }
  SHA1_Update(&_shaCtx, buffer_ptr, bufferLen);
}

/**
 * updates the sha-1 with the data
 *
 * @param buffer_ptr            Buffer
 * @param bufferLen             Buffer Len
 */
void CSha1Data::final()
{
  if(_initialized) {
    _buffer.resize(SHA_DIGEST_LENGTH);
    SHA1_Final(&(_buffer[0]), &_shaCtx);
    
  } else {
    C_THROW("CSha1Data::final: Not initialized");
  }
}

/**
 * Returns the sha-1 buffer
 */
UCharBuffer CSha1Data::getData() const
{
    return _buffer;
}

/*!
  Returns the random bytes
*/
UCharBuffer CSha1Data::getRandomData()
{
  UCharBuffer buffer;
  buffer.resize(SHA_DIGEST_LENGTH);

  int status = RAND_bytes(&(buffer[0]), SHA_DIGEST_LENGTH);

  if (1 != status) {
    string msg  = "Unable to generate random numbers. ";
    msg += ERR_get_error();

    C_THROW(msg);
  }

  return buffer;
}

/*!
  Gets the Sha1 value for the given string
*/
UCharBuffer CSha1Data::getSha1(const string &s)
{
  UCharBuffer b;
  b.resize(SHA_DIGEST_LENGTH);

  SHA1(reinterpret_cast<const uchar*>(s.c_str()), s.length(), &(b[0]));
  return b;
}

/*!
  Returns the Buffer for the hex represenation of sha1
*/
UCharBuffer CSha1Data::fromHexStr(const string &s)
{
  /*if (s.size() != SHA_DIGEST_LENGTH * 2) {
    string msg = "Invalid sha1 string. Length = ";
    msg += CUtil::getString(s.length());
    msg += ". Expected length = ";
    msg += CUtil::getString(SHA_DIGEST_LENGTH * 2);
    C_THROW(msg);
  }*/

  CAutoPtr<BIGNUM> bp;
  BIGNUM *bn = BN_new();
  assert(bn);
  bp = bn;

  ulong e = BN_hex2bn(&bn, s.c_str());
  if (e <= 0) {
    string msg  = "Unable to convert hex string to bignum. ";
    msg += ERR_get_error();

    C_THROW(msg);
  }

  UCharBuffer b;
  b.resize(SHA_DIGEST_LENGTH);
  memset(&(b[0]), 0, b.size());
  int size = BN_num_bytes(bn);

  if (size > 0) {
    int pos = b.size() - size;
    assert(pos >= 0);
    BN_bn2bin(bn, &(b[pos]));
  }
  return b;  
}

//-----------------------------------------------------------------------------
const uint CSha1Data::BUF_SIZE       = 4096;
