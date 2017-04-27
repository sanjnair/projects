/**
 * Class for DES encoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __DESENCDUMP_H__
#define __DESENCDUMP_H__

#include "desbasedump.h"

class DesEncDump : public DesBaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    DesEncDump(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~DesEncDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();

private:
    
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
     * Encrypts the data from input file
     */
    int performEncryption( 
                const unsigned char * sha1_ptr, 
                const unsigned char *desKey_ptr ) const;

    /**
     * Writes the DES encryption header
     *
     */
    int writeEncryptionHeader() const;

    /**
     * Gets the encrypted data for the plain data.
     *
     * @param inBuf_ptr             Input buffer containing plain text
     * @param outBuf_ptr            Output buffer
     * @param bufLen                Length of the buffer
     * @param keySchedule_ptr       Key Schedule
     * @param iv_ptr                Initialization vector
     */
    int getEncryptedData( 
            const unsigned char *inBuf_ptr,
            unsigned char *outBuf_ptr,
            int bufLen,
            DES_key_schedule *keySchedule_ptr,
            unsigned char *iv_ptr ) const;


};

#endif //__DESENCDUMP_H__

