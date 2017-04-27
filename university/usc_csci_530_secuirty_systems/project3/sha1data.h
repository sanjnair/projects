/**
 * Class for Sha-1 storing Sha-1 data
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __SHA1DATA_H__
#define __SHA1DATA_H__

#include <stdio.h>
#include <string>
#include <openssl/sha.h>

class Sha1Data {

public:
    
    /**
     * C'tor.
     */
    Sha1Data();

    /**
     * Virtual D'tor.
     */
    virtual ~Sha1Data();

    /**
     * Calculates SHA-1 on the file pointed by fpSha
     *
     * @param fp                    File pointer
     */
    int calculate( FILE * fp );

    /**
     * updates the sha-1 with the data
     *
     * @param buffer_ptr            Buffer
     * @param bufferLen             Buffer Len
     */
    int update( 
            const unsigned char * buffer_ptr,
            unsigned long bufferLen );

    /**
     * updates the sha-1 with the data
     *
     * @param buffer_ptr            Buffer
     * @param bufferLen             Buffer Len
     */
    int final();

    /**
     * Returns the sha-1 Buffer        
     */
    unsigned char * getBuffer() const;

    /**
     * Returns the Hex representation of the sha-1
     */
    std::string getHexString() const;
    
private:

    unsigned char * _sha1Buffer_ptr;
    SHA_CTX _shaCtx;
    bool _initialized;
};

#endif //__SHA1DATA_H__

