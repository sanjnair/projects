/**
 * Class for DES decoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __DESDECDUMP_H__
#define __DESDECDUMP_H__

#include "desbasedump.h"

class DesDecDump : public DesBaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    DesDecDump(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~DesDecDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();

private:
    
    /**
     * Checks the input file to make sure that it is a valid
     * DES file.
     *
     * @param fileSize              Size of the input file
     */
    int checkInputFile( long fileSize ) const;

    /**
     * Gets the DES key
     * 
     * @param sha1_ptr              Sha-1 value
     * @param desKey_ptr            Des Key
     */
    int getKey( 
            unsigned char * sha1_ptr, 
            unsigned char *desKey_ptr ) const;

    /**
     * Decrypts the data from input file
     *
     * @param sha1_ptr              Sha1 value of the pass-pharase
     * @param desKey_ptr            DES key
     * @param fileSize              Size of the input file
     */
    int performDecryption( 
                const unsigned char * sha1_ptr, 
                const unsigned char *desKey_ptr,
                long fileSize ) const;

    /**
     * Gets the decrypted data for the plain data.
     *
     * @param inBuf_ptr             Input buffer containing cipher text
     * @param outBuf_ptr            Output buffer
     * @param bufLen                Length of the buffer
     * @param keySchedule_ptr       Key Schedule
     * @param iv_ptr                Initialization vector
     */
    int getDecryptedData( 
            const unsigned char *inBuf_ptr,
            unsigned char *outBuf_ptr,
            int bufLen,
            DES_key_schedule *keySchedule_ptr,
            unsigned char *iv_ptr ) const;

    /**
     * Processes the DES header from the DES file
     * 
     * @param fileShaOut_ptr        Hash value of the original file
     * @param numByteLastBlock         Number of bytes padded towads the end of the file
     */
    int processDesHeader( 
            unsigned char *fileSha_ptr,
            unsigned char &numByteLastBlock ) const;

    /**
     * Verifies the DES header
     * 
     * @param buffer_ptr            Buffer
     * @param numByteLastBlock      Number of bytes in the last 8-byte block of the original file
     */
    int verifyDesHeader( 
            const unsigned char *buffer_ptr,
            unsigned char &numByteLastBlock ) const;

    /**
     * Compares the Sha-1 value of original file and Sha-1 value of the
     * decryped file.
     * 
     * @param origSha1              Orignal Sha1 that is stored in the cipher file
     * @param newSha1               Sha1 that is computed for the deciphered text
     *
     * @return                      0 if comparison is successful, else non-zero
     */
    int compareSha1( 
            const unsigned char * origSha1,
            const unsigned char * newSha1 ) const;

};

#endif //__DESDECDUMP_H__

