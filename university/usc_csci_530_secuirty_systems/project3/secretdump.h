/**
 * Class for Secret encoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __SHA1DUMP_H__
#define __SHA1DUMP_H__

#include "basedump.h"
#include <openssl/rsa.h>


class SecretDump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    SecretDump( const std::string filePath, const std::string soPath );

    /**
     * Virtual D'tor.
     */
    virtual ~SecretDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
    
private:

    /**
     * Generate the code for the RSA
     *
     * @param rsa_ptr           RSA pointer
     * @param strCode           code returned
     */
    int getRsaCode( 
                const RSA *rsa_ptr,
                std::string & strCode ) const;

    /**
     * Get the aray code for byte array
     *
     * @param buffer            Byte buffer
     * @param bufferLen         Length of the buffer
     * @param name              Name of the array
     * @param genSizeConst      To generate size constant
     * @param strCode           code returned
     *
     */
    int getArrayCode(
            const unsigned char * buffer,
            int bufferLen,
            const std::string & name,
            bool genSizeConst,
            std::string & strCode ) const; 

    /**
     * Gets the Array code for BIGNUM.
     *
     * @param bigNum_ptr        BIGNUM pointer
     * @param name              Name of the array
     * @param lastNumBytes      Number of bytes to be considered in the last number
     * @param genSizeConst      To generate size constant
     * @param strCode           code returned
     */
    int getArrayCode( 
            const BIGNUM * bigNum_ptr,
            const std::string & name,
            bool genSizeConst,
            int lastNumBytes,
            std::string & strCode ) const; 

    /**
     * Gets the array code for the random numbers
     * 
     * @param name              Name of the array
     * @param strCode           code returned
     */
    int getArrayCodeForRandom( 
            const std::string & name,
            std::string & strCode ) const; 
    
   /**
     * generates the random bytes
     *
     * @param buffer            Buffer to hold data
     * @param bufferLen         Lengh of the buffer
     */
    int genRandomBytes(
            unsigned char * buffer,
            int bufferLen ) const;

};

#endif //__SHA1DUMP_H__

