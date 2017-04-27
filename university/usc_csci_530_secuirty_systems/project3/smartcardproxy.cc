#include "smartcardproxy.h"
#include "hwconstant.h"
#include "hwutil.h"

#include <openssl/err.h>
#include <openssl/ssl.h>


/**
 * C'tor.
 */
SmartCardProxy::SmartCardProxy() { 
    _library = NULL;
    _smartCardState = NULL;
}

/**
 * Virtual D'tor.
 */
SmartCardProxy::~SmartCardProxy() { 
    shutdown();
}

/**
 * Returns 0 if the proxy is initialized.
 */
int SmartCardProxy::isInitialized() const {
    int status = 0;

    if( (NULL == _library) || (NULL == _smartCardState) ) {
        status = HwConstant::ERROR_INVALID_STATE;
        HwUtil::setLastErrorMessage("SmartCardProxy is not initialized");
    }

    return status;
}

/**
 * Shuts down the smart card proxy
 */
int SmartCardProxy::shutdown() {

    int status = 0;

    if( NULL != _library ) {
        status = _library->unload();
        delete _library;
        _library = NULL;
    }

    return status;
}

/**
 * Initializes the Smart card
 *
 * @param soPath        Shared library path
 */
int SmartCardProxy::init( const std::string & soPath ) {

    int status = 0;
    void * symbHandle = NULL;
    SC_Init_Func *pfn = NULL;
    
    if( 0 == status ) {
        _library = new HwLibrary(soPath);

        if( NULL == _library ) {
            status = HwConstant::ERROR_OUT_OF_MEMORY;
            HwUtil::setLastErrorMessage("Unable to create HwLibrary");
        }
    }

    if( 0 == status ) {
        status = _library->load();
    }
    
    if( 0 == status ) {
        status = _library->resolve( HwConstant::SYMBOL_SC_INIT, symbHandle );
        
        if( (0 == status) && (NULL != symbHandle) ) {
            pfn = (SC_Init_Func *) symbHandle;
            _smartCardState = (pfn)();
        }
    }

    if( 0 != status ) {
        shutdown();
    }

    return status;
}

/**
 * Cleans up the Smart card
 */
int SmartCardProxy::cleanup() {

    int status = 0;
    void * symbHandle = NULL;
    SC_Cleanup_Func *pfn = NULL;

    status = isInitialized();

    if( 0 == status ) {
        status = _library->resolve( HwConstant::SYMBOL_SC_CLEANUP, symbHandle );
    }

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_Cleanup_Func *) symbHandle;
        (pfn)(_smartCardState);
    }
    
    shutdown();

    return status;
}

/**
 * Returns the size of the digital signature.
 *
 * @param size              Signature Size
 */
int SmartCardProxy::getSignatureSize( int & size ) const {

    int status = 0;
    void * symbHandle = NULL;
    SC_GetSignatureSize_Func *pfn = NULL;

    status = _library->resolve( HwConstant::SYMBOL_SC_GETSIGNATURESIZE, symbHandle );

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_GetSignatureSize_Func *) symbHandle;

        size = (pfn)(_smartCardState);
        if( -1 == size ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            setLibraryError( HwConstant::SYMBOL_SC_GETSIGNATURESIZE );
        }
    }
    
    return status;
}

/**
 * Signs the Sha1 signature
 *
 * @param sha1_buf              SHA-1 buffer
 * @param buf_return            Return signature
 */
int SmartCardProxy::sign(
        unsigned char sha1_buf[SHA_DIGEST_LENGTH],
        unsigned char * buf_return ) const {

    int status = 0;
    void * symbHandle = NULL;
    SC_Sign_Func *pfn = NULL;

    status = _library->resolve( HwConstant::SYMBOL_SC_SIGN, symbHandle );

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_Sign_Func *) symbHandle;

        status = (pfn)(_smartCardState, sha1_buf, buf_return );
        if( 0 != status ) {
            setLibraryError( HwConstant::SYMBOL_SC_SIGN );
        }
    }
    
    return status;
}

/**
 * Returns the public key size query 
 *
 * @param p_public_exponent_size    Public exponent Size
 * @param p_public_modulus_size     Public modulus Size
 */
int SmartCardProxy::publicSizeQuery(
        int * p_public_exponent_size,
        int * p_public_modulus_size ) const {

    int status = 0;

    void * symbHandle = NULL;
    SC_PublicSizeQuery_Func *pfn = NULL;

    status = _library->resolve( HwConstant::SYMBOL_SC_PUBLICSIZEQUERY, symbHandle );

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_PublicSizeQuery_Func *) symbHandle;

        status = (pfn)(_smartCardState, p_public_exponent_size, p_public_modulus_size );
        if( 0 != status ) {
            setLibraryError( HwConstant::SYMBOL_SC_PUBLICSIZEQUERY );
        }
    }
    
    return status;

}

/**
 * Returns the public key query 
 *
 * @param p_public_exponent_size    Public exponent
 * @param p_public_modulus_size     Public modulus
 */
int SmartCardProxy::publicQuery(
		unsigned char *public_exponent,
        unsigned char *public_modulus) {

    int status = 0;

    void * symbHandle = NULL;
    SC_PublicQuery_Func *pfn = NULL;

    status = _library->resolve( HwConstant::SYMBOL_SC_PUBLICQUERY, symbHandle );

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_PublicQuery_Func *) symbHandle;

        status = (pfn)(_smartCardState, public_exponent, public_modulus );
        if( 0 != status ) {
            setLibraryError( HwConstant::SYMBOL_SC_PUBLICQUERY );
        }
    }

    return status;
}

/**
 * Routine performs secret key generation. 
 * This function returns 0 if successful, otherwise, it should return -1. 
 */
int SmartCardProxy::desKeyGen (
            unsigned char sha1_buf[SHA_DIGEST_LENGTH], 
            unsigned char iv[8],
            unsigned char key1[8], 
            unsigned char key2[8], 
            unsigned char key3[8] ) {

    int status = 0;

    void * symbHandle = NULL;
    SC_3DesKeyGen_Func *pfn = NULL;

    status = _library->resolve( HwConstant::SYMBOL_SC_3DESKEYGEN, symbHandle );

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_3DesKeyGen_Func *) symbHandle;

        status = (pfn)(_smartCardState, sha1_buf, iv, key1, key2, key3 ); 
        if( 0 != status ) {
            setLibraryError( HwConstant::SYMBOL_SC_3DESKEYGEN );
        }
    }

    return status;
}


/**
 * Sets the library error.
 */
void SmartCardProxy::setLibraryError( const std::string & methodName ) const {
    std::string str;

    str  = "SmartCard ";
    str += methodName;
    str += " failed (";
    str += SmartCardProxy::getOpenSslError();
    str += " )";

    HwUtil::setLastErrorMessage( str );
}


/**
 * Gets the open ssl error
 */
std::string SmartCardProxy::getOpenSslError() {

    std::string strError;
    
    unsigned long errorCode = ERR_get_error();

    if( 0 != errorCode ) {
        ERR_load_crypto_strings();
        
        strError  = "Error code = ";
        strError += HwUtil::getString( (int) errorCode );
        strError += ", ";
        
        char * openSslError = ERR_error_string( errorCode, NULL );

        if( NULL == openSslError ) {
            strError += "No information available";
        } else {
            strError += openSslError;
        }
        
        ERR_free_strings();
    }

    return strError;
}
