/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cmd5data.h"
#include "cutil.h"
#include "cconst.h"
#include "cexception.h"

/**
 * C'tor.
 */
CMd5Data::CMd5Data() {
  _md5Buffer = NULL;
  _initialized = false;
}

/**
 * Virtual D'tor.
 */
CMd5Data::~CMd5Data() {
  if (NULL != _md5Buffer) {
    delete _md5Buffer;
    _md5Buffer = NULL;
  }
}

/*!
  updates the md5-1 with the data
*/
void CMd5Data::update(const unsigned char * buf, uint len) {
    if(!_initialized) {
        MD5_Init(&_md5Ctx);
        _initialized = true;
    }
    MD5_Update(&_md5Ctx, buf, len);
}

/*!
  updates the md5-1 with the data
*/
void CMd5Data::final() {

    if(_initialized) {
      _md5Buffer = new unsigned char[MD5_DIGEST_LENGTH];
      MD5_Final(_md5Buffer, &_md5Ctx);
    } else {
        string msg("MD5: Not initialized");
        C_THROW(msg);
    }
}

/**
 * Returns the md5-1 buffer
 */
unsigned char* CMd5Data::getBuffer() const {
    return _md5Buffer;
}

/**
 * Returns the Hex representation of the md5-1
 */
std::string CMd5Data::getHexString() const {
    string str;

    if(NULL != _md5Buffer) {
        str = CUtil::getHexStr(_md5Buffer, MD5_DIGEST_LENGTH);
    }

    return str;
}

