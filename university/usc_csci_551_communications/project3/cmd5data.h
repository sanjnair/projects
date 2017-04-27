/**
 * Class for Md-5 storing Md-5 data
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 */
#ifndef CMD5DATA_H
#define CMD5DATA_H

#include "cglobal.h"
#include <openssl/md5.h>

class CMd5Data
{
public:

    CMd5Data();
    ~CMd5Data();

    void calculate(const string &fPath);
    void update(const uchar *buffer_ptr, ulong bufferLen);
    void final();
    UCharBuffer getData() const;
    string getHexString() const;

    static UCharBuffer getMd5(const string &s);

private:
    UCharBuffer _buffer;
    MD5_CTX _md5Ctx;
    bool _initialized;

    static const uint BUF_SIZE;
};

#endif //CMD5DATA_H

