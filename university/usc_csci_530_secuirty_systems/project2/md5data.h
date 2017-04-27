/**
 * Class for Md-5 storing Md-5 data
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __MD5DATA_H__
#define __MD5DATA_H__

#include <stdio.h>
#include <string>
#include <openssl/md5.h>

class Md5Data {

public:
    
    /**
     * C'tor.
     */
    Md5Data();

    /**
     * Virtual D'tor.
     */
    virtual ~Md5Data();

    /**
     * Calculate the MD5 on the file pointed by the file pointer
     *
     * @param fpMd5                 File pointer
     */
    int calculate( FILE * fpMd5 );

    /**
     * updates the md5 with the data
     *
     * @param buffer_ptr            Buffer
     * @param bufferLen             Buffer Len
     */
    int update( 
            const unsigned char * buffer_ptr,
            unsigned long bufferLen );

    /**
     * updates the md5 with the data
     *
     * @param buffer_ptr            Buffer
     * @param bufferLen             Buffer Len
     */
    int final();

    /**
     * Returns the md5 Buffer        
     */
    unsigned char * getBuffer() const;

    /**
     * Returns the Hex representation of the md5-1
     */
    std::string getHexString() const;
    
private:

    unsigned char * _md5Buffer_ptr;
    MD5_CTX _md5Ctx;
    bool _initialized;
};

#endif //__MD5DATA_H__

