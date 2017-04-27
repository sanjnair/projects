#include "verifydump.h"
#include "hwutil.h"
#include "hwconstant.h"
#include "sha1data.h"
#include "smartcardproxy.h"

/**
 * C'tor.
 *
 * @param filePath          File path
 */
VerifyDump::VerifyDump( const std::string filePath, const std::string soPath ) 
                : BaseDump( filePath, soPath ) {
}

/**
 * Virtual D'tor.
 */
VerifyDump::~VerifyDump() { }

/**
 * Dumps the output (based on input)
 */
int VerifyDump::dump() {

    int status                      = 0;
    int modulusSize                 = 0;
    int exponentSize                = 0;
    int dataLen                     = 0;
    unsigned char *modulus_ptr      = NULL;
    unsigned char *exponent_ptr     = NULL;
    unsigned char *dataBuffer_ptr   = NULL;
    std::string strVerify;
    //-------------------------------------------
    
    status = openInputFile( false );

    if( 0 == status ) {
        status = getPublicKeyInfo( 
                        modulus_ptr, 
                        modulusSize, 
                        exponent_ptr, 
                        exponentSize );
    }
    dataLen = modulusSize * 2;

    if( 0 == status ) {
        status = HwUtil::createBuffer(modulusSize, dataBuffer_ptr );
    }

    if( 0 == status ) {
        
        status = getEncyptedData( dataLen, dataBuffer_ptr );
    }

    if( 0 == status ) {
        status = getVerifyedStr(
                        dataBuffer_ptr,
                        modulus_ptr,
                        modulusSize,
                        exponent_ptr,
                        exponentSize,
                        strVerify );
    }

    if( 0 == status ) {
        HwUtil::writeOutputLn( strVerify );
    }

    HwUtil::deleteBuffer( modulus_ptr );
    HwUtil::deleteBuffer( exponent_ptr );
    HwUtil::deleteBuffer( dataBuffer_ptr );

    closeInputFile();

    return status;
}

/**
 * Gets the encrypted data.
 *
 * @param dataLen               [in] Length of data
 * @param dataBuffer            [out] Buffer containing data.
 *                                    Buffer should be allocated to dataLen / 2.
 *                              
 */
int VerifyDump::getEncyptedData( 
            int dataLen,
            unsigned char * dataBuffer ) const {

    int status      = 0;
    int readLen     = 0;
    unsigned char * buffer_ptr= NULL;
    
    status = HwUtil::createBuffer( dataLen, buffer_ptr );

    if( 0 == status ) {
        readLen = fread( buffer_ptr, 
                         sizeof(unsigned char), 
                         dataLen,
                         _fp );

        if( readLen != dataLen ) {
            std::string msg = "Invalid number of bytes returned from file. Extected = ";
            msg += HwUtil::getString( dataLen );
            msg += ", Actual = ";
            msg += HwUtil::getString( readLen );
    
            HwUtil::setLastErrorMessage( msg );
            status = HwConstant::ERROR_FILE_READ;
        }
    }

    if( 0 == status ) {
        char * str_ptr = NULL;
        status = HwUtil::createCharBuffer( dataLen + 1, str_ptr );

        if( 0 == status ) {
            memcpy(str_ptr, buffer_ptr, dataLen );
            str_ptr[dataLen] = '\0';
        }

        if( 0 == status ) {
            status = getOctetArray( 
                std::string(str_ptr), 
                            dataLen, 
                            dataBuffer );
        }

        HwUtil::deleteCharBuffer( str_ptr );
    }

    HwUtil::deleteBuffer( buffer_ptr );

    return status;
}

/**
 * Returns the octet array for the given hex string.
 * 
 * @param hexString             [in] Hex String
 * @param dataLen               [in] Expected data length
 * @param dataBuffer            [out] converted buffer
 */
int VerifyDump::getOctetArray( 
        const std::string & hexString,
        int dataLen,
        unsigned char * dataBuffer ) const {
    
    int status = 0;
    int size = 0;
    BIGNUM *bigNum_ptr = NULL;
    std::string strError;
    
    bigNum_ptr = BN_new();

    if( NULL == bigNum_ptr ) {
        status = HwConstant::ERROR_OUT_OF_MEMORY;
        HwUtil::setLastErrorMessage("Unable to create BIGNUM");
    }

    if( 0 == status ) {
        size = BN_hex2bn( &bigNum_ptr, hexString.c_str() );
        
        if( 0 == size ) {
            strError = "Invalid input: unable to convert hex number to byte array. 0 returned";
        } else if( size > dataLen ) {
            strError = "Invalid input: unable to convert hex number to byte array. expcted = ";
            strError += HwUtil::getString( dataLen );
            strError += " actual = ";
            strError += HwUtil::getString( size );
        }

        if( strError.length() > 0 ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            HwUtil::setLastErrorMessage(strError);
        }
    }

    if( 0 == status ) {
        size = BN_bn2bin(bigNum_ptr, dataBuffer);

        if( size != (dataLen/2)  ) {
            strError = "BN_bn2bin did not return correct bytes. (size = ";
            strError += HwUtil::getString( size );
            strError += ", dataLen = ";
            strError += HwUtil::getString( dataLen/2 );
            strError += " )";

            HwUtil::setLastErrorMessage( strError );
            status = HwConstant::ERROR_INVALID_INPUT;
        }
    }

    if( NULL != bigNum_ptr ) {
        BN_free(bigNum_ptr);
        bigNum_ptr = NULL;
    }

    return status;
}


/**
 * Gets the signed SHA-1 signature for the file.
 *
 * @param data_ptr              [in] Data
 * @param modulus_ptr           [in] Modulus 
 * @param modulusSize           [in] Modulus size
 * @param exponent_ptr          [in] Exponent 
 * @param exponentSize          [in] Exponent size
 * @param strVerify             [out]Verifyed string
 */
int VerifyDump::getVerifyedStr( 
                    unsigned char *data_ptr,
                    const unsigned char *modulus_ptr,
                    int modulusSize,
                    const unsigned char *exponent_ptr,
                    int exponentSize,
                    std::string & strVerify ) const {

    int status = 0;
    int size = 0;
    RSA * rsa = NULL;
    unsigned char * buf_return = NULL;

    status = HwUtil::createBuffer( size, buf_return );

    status = getNewRsa( 
                modulus_ptr, 
                modulusSize, 
                exponent_ptr, 
                exponentSize, 
                rsa ); 

    if( 0 == status ) {
        size = RSA_size(rsa);

        if( size <= 0 ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            HwUtil::setLastErrorMessage("Invalid RSA size (0) ");
        }
    }

    if( 0 == status ) {
        status = HwUtil::createBuffer( size, buf_return );
    }

    if( 0 == status ) {
        size = RSA_public_decrypt( 
                    modulusSize, 
                    data_ptr, 
                    buf_return, 
                    rsa, 
                    RSA_PKCS1_PADDING );

        if( -1 == size ) {
            std::string msg = "Error calling RSA_public_decrypt. ";
            msg += SmartCardProxy::getOpenSslError();
            HwUtil::setLastErrorMessage( msg );
            status = -1;
        }
    }

    if( 0 == status ) {
        strVerify = HwUtil::getHexString( buf_return, SHA_DIGEST_LENGTH );
    }
   
    HwUtil::deleteBuffer( buf_return );
    if( NULL != rsa ) {
        RSA_free( rsa );
    }

    return status;
}

/**
 * Gets the public key info.
 *
 * @param modulus_ptr           [out] Modulus 
 * @param modulusSize           [out] Modulus size
 * @param exponent_ptr          [out] Exponent 
 * @param exponentSize          [out] Exponent size
 */
int VerifyDump::getPublicKeyInfo(
        unsigned char *& modulus_ptr,
        int & modulusSize,
        unsigned char *& exponent_ptr,
        int & exponentSize ) const {

    int status = 0;
    
    SmartCardProxy proxy;

    status = proxy.init( _soPath );
    
    if( 0 == status ) {
        status = proxy.publicSizeQuery( &exponentSize, &modulusSize );
    }

    if( 0 == status ) {
        status = HwUtil::createBuffer( modulusSize, modulus_ptr );
    }
    if( 0 == status ) {
        status = HwUtil::createBuffer( exponentSize, exponent_ptr );
    }

    if( 0 == status ) {
        status = proxy.publicQuery( exponent_ptr, modulus_ptr );
    }
   
    if( 0 == status ) {
        if( (NULL == exponent_ptr) || (NULL == modulus_ptr) ) {
            HwUtil::setLastErrorMessage("Invalid exponent of modulus ");
            status = HwConstant::ERROR_OUT_OF_MEMORY;
        }
    }

    if( 0 == status ) {
        if( SHA_DIGEST_LENGTH > (modulusSize) ) {
            std::string msg = "Invalid modulus size. Expected = ";
            msg += HwUtil::getString(modulusSize);
            msg += ", Actual = ";
            msg += HwUtil::getString(SHA_DIGEST_LENGTH);

            HwUtil::setLastErrorMessage( msg );
            status = HwConstant::ERROR_INVALID_INPUT;
        }
    }

    return status;
}

/**
 * Creates a new RSA and initializes it with the keys.
 * CALLER MUST CALL RSA_free(RSA *rsa) to free the memory created by 
 * this call.
 *
 * If RSA cannot be created, this method will return -1. Else 0.
 *
 * @param modulus_ptr           [in] Modulus 
 * @param modulusSize           [in] Modulus size
 * @param exponent_ptr          [in] Exponent 
 * @param exponentSize          [in] Exponent size
 * @param rsa                   [out] RSA to be created
 */ 
int VerifyDump::getNewRsa( 
        const unsigned char * modulus_ptr,
        int modulusSize,
        const unsigned char * exponent_ptr,
        int exponentSize,
        RSA *& rsa ) const {

    int status = 0;

    rsa = RSA_new();

    if( NULL != rsa ) {
        rsa->n = BN_bin2bn( modulus_ptr, modulusSize, rsa->n );
        rsa->e = BN_bin2bn( exponent_ptr, exponentSize, rsa->e );
    } else {
        HwUtil::setLastErrorMessage(" Unable to create new rsa structure");
        status = HwConstant::ERROR_OUT_OF_MEMORY;
    }

    return status;
}

