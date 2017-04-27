/**
 * Class for Encrypting the DES data
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __DESENCRYPTOR_H__
#define __DESENCRYPTOR_H__

#include "desbase.h"

class DesEncryptor : public DesBase {

public:
    
    /**
     * C'tor.
     */
    DesEncryptor();

    /**
     * Virtual D'tor.
     */
    virtual ~DesEncryptor();

    /**
     * Intializes the des
     *
     * @param soPath            SoPath
     * @param filePath          File Path
     */
    int init( 
        const std::string & soPath,
        const std::string & filePath );

    /**
     * encrypts the data and write to fp
     *
     * @param fpFrom            From file
     * @param fpTo              To File
     */
    int encrypt(
            FILE * fpFrom,
            FILE * fpTo );

    /**
     * encrypts the data and write to fp
     *
     * @param fileFrom            From file
     * @param fileTo              To File
     */
    int encrypt(
            const std::string & fileFrom,
            const std::string & fileTo );

    /**
     * Sets the password
     *
     * @param password
     */
    void setPwd( const std::string & password );

protected:

    /**
     * Intitializes the DES keys
     *
     * @param sha1Buf               Sha1 value
     * @param soPath                So Path
     */
    int initDesKeys( 
            unsigned char sha1Buf[SHA_DIGEST_LENGTH],
            const std::string & soPath );

private:

    /**
     * encrypts the data 
     */
    int encrypt();

    /**
     * Gets the header
     *
     * @param fp                File pointer to write to
     */
    int writeHeader( FILE * fp ) const;

    /**
     * Checks to see if the key is valid
     * 
     * @param ks                DES key schedule
     */
    int validateKey( DES_key_schedule *ks ) const;

    std::string _pwd;
};

#endif //__DESENCRYPTOR_H__

