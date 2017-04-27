/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CMD5DATA_H
#define CMD5DATA_H

#include "cglobal.h"
#include <openssl/md5.h>

class CMd5Data {

public:

    CMd5Data();
    ~CMd5Data();

    void update(const unsigned char * buf, uint len);
    void final();
    unsigned char *getBuffer() const;
    string getHexString() const;

private:
    unsigned char * _md5Buffer;
    MD5_CTX _md5Ctx;
    bool _initialized;
};

#endif //CMD5DATA_H

