/**
 * Class for Verify Dump
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/18/2006 - Created
 */
#ifndef __VERIFYDUMP_H__
#define __VERIFYDUMP_H__

#include "basedump.h"
#include <openssl/rsa.h>


class VerifyDump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    VerifyDump( const std::string filePath, const std::string soPath );

    /**
     * Virtual D'tor.
     */
    virtual ~VerifyDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
    
private:

    /**
     * Gets the encrypted data.
     *
     * @param dataLen               [in] Length of data
     * @param dataBuffer            [out] Buffer containing data.
     *                                    Buffer should be allocated to dataLen /2
     *                              
     */
    int getEncyptedData( 
                int dataLen,
                unsigned char * dataBuffer ) const;

    
    /**
     * Returns the octet array for the given hex string.
     * 
     * @param hexString             [in] Hex String
     * @param dataLen               [in] Expected data length
     * @param dataBuffer            [out] converted buffer
     */
    int getOctetArray( 
            const std::string & hexString,
            int dataLen,
            unsigned char * dataBuffer ) const;

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
    int getVerifyedStr( 
            unsigned char *data_ptr,
            const unsigned char * modulus_ptr,
            int modulusSize,
            const unsigned char * exponent_ptr,
            int exponentSize,
            std::string & strVerify ) const;

    /**
     * Gets the public key info.
     *
     * @param modulus_ptr           [out] Modulus 
     * @param modulusSize           [out] Modulus size
     * @param exponent_ptr          [out] Exponent 
     * @param exponentSize          [out] Exponent size
     */
    int getPublicKeyInfo(
            unsigned char *& modulus_ptr,
            int & modulusSize,
            unsigned char *& exponent_ptr,
            int & exponentSize ) const;

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
    int getNewRsa( 
            const unsigned char * modulus_ptr,
            int modulusSize,
            const unsigned char * exponent_ptr,
            int exponentSize,
            RSA *& rsa ) const; 

};

#endif //__VERIFYDUMP_H__

