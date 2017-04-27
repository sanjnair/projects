#include "sha1data.h"
#include "hwconstant.h"
#include "hwutil.h"

/**
 * C'tor.
 */
Sha1Data::Sha1Data() {
    _sha1Buffer_ptr = NULL;
    _initialized = false;
}

/**
 * Virtual D'tor.
 */
Sha1Data::~Sha1Data() {
    HwUtil::deleteBuffer( _sha1Buffer_ptr );
}

/**
 * Calculates SHA-1 on the file pointed by fp
 *
 * @param fp                    File pointer
 */
int Sha1Data::calculate( FILE * fp ) {

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
 * updates the sha-1 with the data
 *
 * @param buffer_ptr            Buffer
 * @param bufferLen             Buffer Len
 */
int Sha1Data::update( 
        const unsigned char * buffer_ptr,
        unsigned long bufferLen ) {

    int status = 0;

    if( !_initialized ) {
        SHA1_Init( &_shaCtx );
        _initialized = true;
    }
    
    SHA1_Update( &_shaCtx, buffer_ptr, bufferLen );

    return status;
}

/**
 * updates the sha-1 with the data
 *
 * @param buffer_ptr            Buffer
 * @param bufferLen             Buffer Len
 */
int Sha1Data::final() {

    int status = 0;

    if( _initialized ) {
        if( NULL == _sha1Buffer_ptr ) {
            status = HwUtil::createBuffer( SHA_DIGEST_LENGTH, _sha1Buffer_ptr );
        }

        if( 0 == status ) {
            SHA1_Final( _sha1Buffer_ptr, &_shaCtx );
        }
    } else {
        status = HwConstant::ERROR_INVALID_STATE;
        HwUtil::setLastErrorMessage("Sha1Data::final: Not initialized");
    }

    return status;
}

/**
 * Returns the sha-1 buffer        
 */
unsigned char * Sha1Data::getBuffer() const {
    return _sha1Buffer_ptr;
}

/**
 * Returns the Hex representation of the sha-1
 */
std::string Sha1Data::getHexString() const {
    std::string str;

    if( NULL != _sha1Buffer_ptr ) {
        str = HwUtil::getHexString( _sha1Buffer_ptr, SHA_DIGEST_LENGTH );
    }

    return str;
}

