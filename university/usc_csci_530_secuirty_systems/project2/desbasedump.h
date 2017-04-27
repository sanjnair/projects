/**
 * Base class for DES dumps
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */

#ifndef __DESBASEDUMP_H__
#define __DESBASEDUMP_H__

#include "basedump.h"
#include <openssl/des.h>
#include <openssl/sha.h>


class DesBaseDump : public BaseDump {

public:
    
    /**
     * C'tor.
     *
     * @param filePath          File path
     */
    DesBaseDump(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~DesBaseDump();

protected:
    
    /**
     * Opens the input file. If the file path is valid and open is successful,
     * this methid creates a valid file pointer and keeps it. In this case, the
     * return code will be 0. Otherwise, return code will be non-zero.
     * 
     */
    virtual int openInputFile();

    /**
     * Returns the file size. If error occurs, error is recorded.
     *
     */
    int getFileSize( long & fileSize ) const;

    /**
     * Gets the password from the user and returns the SHA-1 Hash
     * of the password.
     *
     * @param sha1_ptr              Returns the Sha1 value of the password
     * @param encrypt               If true, password will be verified.
     *
     * @return                      0 if OK, else non-zero value
     */
    virtual int getPassword( 
                    unsigned char *sha1_ptr, 
                    bool encrypt ) const;

    /**
     * Returns DES prompt string
     *
     * @param encrypt               Encrption of decryption
     */
    virtual std::string getDesPrompt( bool encrypt ) const;

    /**
     * Returns the sha1 value for the string
     *
     * @param str                   String for the sha1 hash to be computed
     * @param sha1_ptr              Sha-1 output
     */
    virtual int getSha1FromStr( 
                    const std::string & str, 
                    unsigned char *sha1_ptr ) const;

    /**
     * Gets the DES key from the sha-1 value
     *
     * @param sha1_ptr              Sha-1 value
     * @param desKey_ptr            DES key block
     */
    virtual int getDesKey( 
                    const unsigned char *sha1_ptr, 
                    unsigned char *desKey_ptr ) const;

    /**
     * Returns the initialization vector for the DES encryption
     * 
     * @param sha1_ptr              Sha1 value
     * @param iv_ptr                Intitialization vector 
     */
    int getIv( 
            const unsigned char * sha1_ptr, 
            unsigned char * iv_ptr ) const;

    /**
     * Returns DES key schedule
     * 
     * @param desKey_ptr            des key
     * @param desKeySched_out_ptr   DES key schedule 
     */
    int getDesKeySchedule( 
            const unsigned char * desKey_ptr,
            DES_key_schedule * desKeySched_out_ptr ) const;

};

#endif //__DESBASEDUMP_H__

