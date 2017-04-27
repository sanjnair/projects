#include "md5data.h"
#include "hwconstant.h"
#include "hwutil.h"

/**
 * C'tor.
 */
Md5Data::Md5Data() {
    _md5Buffer_ptr = NULL;
    _initialized = false;
}

/**
 * Virtual D'tor.
 */
Md5Data::~Md5Data() {
    HwUtil::deleteBuffer( _md5Buffer_ptr );
}

/**
 * Calculates MD5-1 on the file pointed by fp
 *
 * @param fp                    File pointer
 */
int Md5Data::calculate( FILE * fp ) {

    int status  = 0;
    int readLen = 0;
    
    unsigned char *buffer_ptr   = NULL;

    status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, buffer_ptr );
    
    if( 0 == status ) {

        while( (!feof(fp)) && (0 == status) ) {
            memset(buffer_ptr, 0, HwConstant::BUFFER_SIZE );
            readLen = fread( buffer_ptr, 
                             sizeof(unsigned char), 
                             HwConstant::BUFFER_SIZE,
                             fp );

            status = HwUtil::checkFileReadStatus( fp );
                
            if( (0 == status) && (readLen > 0)  ) {
                update( buffer_ptr, readLen ); 
            }
        }

        HwUtil::deleteBuffer( buffer_ptr );

        if( 0 == status ) {
            final();
        }
    }
    
    return status;
}


/**
 * updates the md5-1 with the data
 *
 * @param buffer_ptr            Buffer
 * @param bufferLen             Buffer Len
 */
int Md5Data::update( 
        const unsigned char * buffer_ptr,
        unsigned long bufferLen ) {

    int status = 0;

    if( !_initialized ) {
        MD5_Init( &_md5Ctx );
        _initialized = true;
    }
    
    MD5_Update( &_md5Ctx, buffer_ptr, bufferLen );

    return status;
}

/**
 * updates the md5-1 with the data
 *
 * @param buffer_ptr            Buffer
 * @param bufferLen             Buffer Len
 */
int Md5Data::final() {

    int status = 0;

    if( _initialized ) {
        if( NULL == _md5Buffer_ptr ) {
            HwUtil::createBuffer( MD5_DIGEST_LENGTH, _md5Buffer_ptr );
        }
        MD5_Final( _md5Buffer_ptr, &_md5Ctx );

    } else {
        status = HwConstant::ERROR_INVALID_STATE;
    }

    return status;
}

/**
 * Returns the md5-1 buffer        
 */
unsigned char * Md5Data::getBuffer() const {
    return _md5Buffer_ptr;
}

/**
 * Returns the Hex representation of the md5-1
 */
std::string Md5Data::getHexString() const {
    std::string str;

    if( NULL != _md5Buffer_ptr ) {
        str = HwUtil::getHexString( _md5Buffer_ptr, MD5_DIGEST_LENGTH );
    }

    return str;
}

