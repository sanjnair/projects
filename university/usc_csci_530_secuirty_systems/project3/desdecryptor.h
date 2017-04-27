/**
 * Class for Decrypting the DES data
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __DESDECRYPTOR_H__
#define __DESDECRYPTOR_H__

#include "desbase.h"

class DesDecryptor : public DesBase {

public:
    
    /**
     * C'tor.
     */
    DesDecryptor();

    /**
     * Virtual D'tor.
     */
    virtual ~DesDecryptor();

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
     * decrypts the data and write to fp
     *
     * @param fpFrom            From file
     * @param fpTo              To File
     */
    int decrypt(
            FILE * fpFrom,
            FILE * fpTo );

    /**
     * encrypts the data and write to fp
     *
     * @param fileFrom          From file
     * @param fileTo            To File
     */
    int decrypt(
            const std::string & fileFrom,
            const std::string & fileTo );

    /**
     * Returns the password
     */
    std::string getPwd() const;

private:

    /**
     * encrypts the data 
     */
    int decrypt();

    /**
     * Checks the input file to make sure that it is a valid
     * DES file.
     *
     * @param fileSize              Size of the input file
     */
    int checkInputFile( long fileSize ) const;

    /**
     * Processes the DES header from the DES file
     * 
     * @param fp                        File pointer
     * @param numByteLastBlock          Number of bytes padded towads the end of the file
     */
    int processDesHeader( 
            FILE * fp,
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

    std::string _pwd;
};

#endif //__DESDECRYPTOR_H__

