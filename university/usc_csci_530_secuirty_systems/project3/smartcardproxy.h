/**
 * Class for Smart Card Proxy,
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __SMARTCARD_PROXY_H__
#define __SMARTCARD_PROXY_H__

#include "hwlibrary.h"
#include <openssl/sha.h>

extern "C" {
#include "scintrf.h"
}

class SmartCardProxy {

public:
    
    /**
     * C'tor.
     */
    SmartCardProxy();

    /**
     * Virtual D'tor.
     */
    virtual ~SmartCardProxy();

    /**
     * Initializes the Smart card
     *
     * @param soPath        Shared library path
     */
    int init( const std::string & soPath );

    /**
     * Cleans up the Smart card
     */
    int cleanup();

    /**
     * Returns the size of the digital signature.
     * @param size              Signature Size
     */
    int getSignatureSize(  int & size ) const;

    /**
     * Signs the Sha1 signature
     *
     * @param sha1_buf              SHA-1 buffer
     * @param buf_return            Return signature
     */
    int sign(
            unsigned char sha1_buf[SHA_DIGEST_LENGTH],
		    unsigned char * buf_return ) const;

    /**
     * Returns the public key size query 
     *
     * @param p_public_exponent_size    Public exponent Size
     * @param p_public_modulus_size     Public modulus Size
     */
    int publicSizeQuery(
            int * p_public_exponent_size,
            int * p_public_modulus_size ) const;
    

    /**
     * Returns the public key query 
     *
     * @param p_public_exponent_size    Public exponent
     * @param p_public_modulus_size     Public modulus
     */
    int publicQuery(
		    unsigned char *public_exponent,
		    unsigned char *public_modulus);

    /**
     * Routine performs secret key generation. 
     * This function returns 0 if successful, otherwise, it should return -1. 
     */
    int desKeyGen (
            unsigned char sha1_buf[SHA_DIGEST_LENGTH], 
            unsigned char iv[8],
            unsigned char key1[8], 
            unsigned char key2[8], 
            unsigned char key3[8] );

    /**
     * Gets the open ssl error
     */
    static std::string getOpenSslError();

private:
    
    /**
     * Shuts down the smart card proxy
     */
    int shutdown();
    
    /**
     * Returns 0 if the proxy is initialized.
     */
    int isInitialized() const;
    /**
     * Sets the library error.
     */
    void setLibraryError( const std::string & methodName ) const;

    SmartcardState _smartCardState;
    HwLibrary * _library;
};

#endif //__SMARTCARD_PROXY_H__

