/**
 * Class for Sha-1 storing Sha-1 data
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 */
#ifndef CSHA1DATA_H
#define CSHA1DATA_H

#include <openssl/sha.h>
#include "cglobal.h"

class CSha1Data
{
public:
  CSha1Data();
  ~CSha1Data();

  void calculate(const string &fPath);
  void update(const uchar *buffer_ptr, ulong bufferLen);
  void final();
  UCharBuffer getData() const;

  static UCharBuffer getRandomData();
  static UCharBuffer getSha1(const string &s);
  static UCharBuffer fromHexStr(const string &s);
  
private:
  UCharBuffer _buffer;
  SHA_CTX _shaCtx;
  bool _initialized;

  static const uint BUF_SIZE;
};

#endif //CSHA1DATA_H

