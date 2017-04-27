/**
 * DES base class
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __DESBASE_H__
#define __DESBASE_H__

#include <string>
#include <openssl/sha.h>
#include <openssl/des.h>

class DesBase {

public:

    /**
     * Virtual D'tor.
     */
    virtual ~DesBase();

protected:
    
    /**
     * C'tor.
     */
    DesBase();

    /**
     * Returns DES prompt string
     *
     * @param encrypt               Encrption of decryption
     */
    std::string getDesPrompt( bool encrypt ) const;

    /**
     * Returns the sha1 value for the string
     *
     * @param str                   String for the sha1 hash to be computed
     * @param sha1Buf               Sha-1 output
     */
    int getSha1FromStr( 
                    const std::string & str, 
                    unsigned char sha1Buf[SHA_DIGEST_LENGTH] ) const;

    /**
     * Gets the password from the user and returns the SHA-1 Hash
     * of the password.
     *
     * @param encrypt               If true, password will be verified.
     * @param password              Password
     * @param sha1_ptr              Returns the Sha1 value of the password
     *
     * @return                      0 if OK, else non-zero value
     */
    int getPassword( 
            bool encrypt,
            std::string & password,
            unsigned char sha1Buf[SHA_DIGEST_LENGTH] ) const;

    /**
     * Intitializes the DES keys
     *
     * @param sha1Buf               Sha1 value
     * @param soPath                So Path
     */
    int initDesKeys( 
            unsigned char sha1Buf[SHA_DIGEST_LENGTH],
            const std::string & soPath );


    /**
     * Returns DES key schedule
     * 
     * @param key                   des key
     * @param ks                    
     */
    int getDesKeySchedule( 
            const unsigned char key[8],
            DES_key_schedule *ks ) const;

    /**
     * Returns the file size. If error occurs, error is recorded.
     *
     * @param fileName              Name of the file
     * @param fileSize              Size of the file
     */
    int getFileSize( 
             const std::string & fileName,
             long & fileSize ) const;

    /**
     * Validates the init
     */
    int validateInit() const;
    
    //-------------------------------------------------------------------------
    long _fileSize;
    bool _initialized;
    
    std::string _fileFrom;
    std::string _fileTo;
    FILE * _fpFrom;
    FILE * _fpTo;
    
    unsigned char _iv[8];
    DES_key_schedule _ks1;
    DES_key_schedule _ks2;
    DES_key_schedule _ks3;
};

#endif //__DESBASE_H__

