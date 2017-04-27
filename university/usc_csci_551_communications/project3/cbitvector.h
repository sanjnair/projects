/**
 * Bit Vector implementation
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 */
#ifndef CBITVECTOR_H
#define CBITVECTOR_H

#include <openssl/bn.h>
#include "cglobal.h"
#include "cautoptr.h"

class CBitVector
{
public:

  CBitVector();
  CBitVector(BIGNUM *b);

  uint size() const;
  void clear();
  
  void setBit(uint i);
  void clearBit(uint i);
  bool isBitSet(uint i);

  UCharBuffer getBuffer() const;
  void setBloomFilter(const StringVector &v);
  void setBloomFilter(const string &s);

  UCharBuffer operator&(const CBitVector &rhs);

  string toHexStr() const;
  static CBitVector fromHexStr(const string &s);
  static CBitVector fromBuffer(const UCharBuffer &b);

  static const uint BIT_SIZE;
  static const uint BYTE_SIZE;  
    
private:
  void resize(int bits);
  void setBloomFilter(const UCharBuffer &p, bool sha1);
  void errorIfInvalidIndex(uint i) const;
  static void errorIfOpenSslError(ulong e, const string &m);

  CAutoPtr<BIGNUM> _bufP;
};

#endif //CBITVECTOR_H

