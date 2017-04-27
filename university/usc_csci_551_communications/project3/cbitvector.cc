/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <openssl/err.h>
#include "cbitvector.h"
#include "cutil.h"
#include "csha1data.h"
#include "cmd5data.h"

/*!
    Creates an empty CBitVector.
*/
CBitVector::CBitVector()
{
  _bufP = BN_new();
  assert(_bufP.get());
  
  resize(BIT_SIZE);
  ERR_load_crypto_strings();
}

/*!
  Creates the BitVector given the BIG NUM
  This class will own the pointer b
*/
CBitVector::CBitVector(BIGNUM *b)
{
  assert(b);

  _bufP = b;
  resize(BIT_SIZE);
  ERR_load_crypto_strings();
}

/*!
  Resizes the Bit vector
*/
void CBitVector::resize(int bits)
{
  BIGNUM *b = bn_expand(_bufP.get(), bits);
  assert(b);
}

/*!
  Returns the size of the Bit Vector
*/
uint CBitVector::size() const
{
  return BIT_SIZE;
}

/*!
  Clears the bit vector
*/
void CBitVector::clear()
{
  int size = BN_num_bytes(_bufP.get());
  if (size > 0) {
    UCharBuffer b;
    b.resize(size);
    BN_bin2bn(&(b[0]), b.size(), _bufP.get());
  }
}

/*!
  Sets the bit identified by the index
*/
void CBitVector::setBit(uint i)
{
  errorIfInvalidIndex(i);

  ulong e = BN_set_bit(_bufP.get(), i);
  errorIfOpenSslError(e, "setBit");
}

/*!
  Sets the bit identified by the index
*/
void CBitVector::clearBit(uint i)
{
  errorIfInvalidIndex(i);

  ulong e = BN_clear_bit(_bufP.get(), i);
  errorIfOpenSslError(e, "clearBit");

}

/*!
  Returns true if the bit is set
*/
bool CBitVector::isBitSet(uint i)
{
  errorIfInvalidIndex(i);

  return BN_is_bit_set(_bufP.get(), i);
}

/*!
  Returns the UCharBuffer
*/
UCharBuffer CBitVector::getBuffer() const
{
  UCharBuffer b;
  b.resize(BYTE_SIZE);
  memset(&(b[0]), 0, b.size());

  int size = BN_num_bytes(_bufP.get());

  if (size > 0) {
    int pos = b.size() - size;
    assert(pos >= 0);
    BN_bn2bin(_bufP.get(), &(b[pos]));
  }
  return b;
}

/*!
  Sets the bloom filter
*/
void CBitVector::setBloomFilter(const StringVector &v)
{
  for (uint i=0; i<v.size(); i++) {
    setBloomFilter(v.at(i));    
  }
}

/*!
  Sets the bloom filter
*/
void CBitVector::setBloomFilter(const string &s)
{
  string str = CUtil::toLower(s);
  setBloomFilter(CSha1Data::getSha1(str), true);
  setBloomFilter(CMd5Data::getMd5(str), false);
}

/*!
  \internal
  Sets the bloom filter for the given buffer
*/
void CBitVector::setBloomFilter(
  const UCharBuffer &p, bool sha1)
{
  uint size = p.size();
  ushort data = (p[size-2] & 0x01) << 8;
  data += p[size-1];

  int pos = sha1 ? (512 + data) : data;
  setBit(pos);
}

/*!
  And operator
*/
UCharBuffer CBitVector::operator&(const CBitVector &rhs)
{
  UCharBuffer srcB = getBuffer();
  UCharBuffer dstB = rhs.getBuffer();

  if (srcB.size() != dstB.size()) {
    string msg="Unable to perform AND operation. Src size = ";
    msg += CUtil::getString(srcB.size()) + string(" dest size = ");
    msg += CUtil::getString(dstB.size());
    C_THROW(msg);
  }

  UCharBuffer retB(srcB.size());

  for (uint i=0; i<srcB.size(); i++) {
    retB[i] = srcB.at(i) & dstB.at(i);
  }

  return retB;  
}

/*!
  Returns the hex string format
*/
string CBitVector::toHexStr() const
{
  return CUtil::getHexStr(getBuffer());
}

/*!                         
  Constructs a bit vector from the hex string
*/
CBitVector CBitVector::fromHexStr(const string &s)
{
  CAutoPtr<BIGNUM> bp;
  BIGNUM *bn = BN_new();
  assert(bn);
  bp = bn;

  ulong e = BN_hex2bn(&bn, s.c_str());
  errorIfOpenSslError(e, "fromHexStr");

  CBitVector bv(bp.release());
  return bv;
}

/*!
  Constructs a bit vector from the Buffer
*/
CBitVector CBitVector::fromBuffer(const UCharBuffer &b)
{
  if (b.size() != static_cast<uint>(BYTE_SIZE)) {
    string msg = "Invalid buffer size ";
    msg += CUtil::getString(b.size());
    C_THROW(msg);
  }

  CAutoPtr<BIGNUM> bp;
  BIGNUM *bn = BN_new();
  assert(bn);
  bp = bn;

  BN_bin2bn(&(b[0]), b.size(), bn);

  CBitVector bv(bp.release());
  return bv;
}

//-----------------------------------------------------------------------------

/*!
  \internal
  Throws exception if index is invalid
*/
void CBitVector::errorIfInvalidIndex(uint i) const
{
  if ((i < 0) || (i >= size())) {
    string msg = "Invalid index provided (valid = 0-";
    msg += CUtil::getString(size()-1);
    msg += " provided = ";
    msg += CUtil::getString(i);
    msg += ")";
    C_THROW(msg);
  }
}

/*!
  \internal
  Throws exception if the error code is invalid
*/
void CBitVector::errorIfOpenSslError(
  ulong e, const string &m)
{
  if (0 == e) {
    char *reason = ERR_error_string(e, NULL);

    string msg = "OpenSSL error while executing ";
    msg += m;
    msg += " Error code = ";
    msg += CUtil::getString(e);
    msg += " Reason = ";

    if (reason) {
      msg += reason;
      OPENSSL_free(reason);
    } else {
      msg += "Not avaiable";
    }
  }
}

//-----------------------------------------------------------------------------
const uint CBitVector::BIT_SIZE       = 1024;
const uint CBitVector::BYTE_SIZE      = BIT_SIZE/8;
