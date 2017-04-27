#include "desdecdump.h"
#include "hwconstant.h"
#include "hwutil.h"
#include "sha1data.h"

/**
 * C'tor.
 *
 * @param filePath          File path
 */
DesDecDump::DesDecDump(std::string filePath) : DesBaseDump(filePath) {
    
}

/**
 * Virtual D'tor.
 */
DesDecDump::~DesDecDump() {}

/**
 * Dumps the output (based on input file or stdin)
 */
int DesDecDump::dump() {

    int status = 0;
    long fileSize = 0;
    unsigned char *sha1_ptr     = NULL;
    unsigned char *desKey_ptr   = NULL; //typedef unsigned char DES_cblock[8];

    status = openInputFile();

    if( 0 == status ) {
        status = getFileSize( fileSize );

        if( 0 == status ) {
            status = checkInputFile( fileSize );
        }

        if( 0 == status ) {
            status = HwUtil::createBuffer( SHA_DIGEST_LENGTH, sha1_ptr );
        }
        if( 0 == status ) {
            status = HwUtil::createBuffer( HwConstant::DES_KEY_LENGTH, desKey_ptr );
        }
        if( 0 == status ) {
            status = getKey( sha1_ptr, desKey_ptr );
        }
        if( 0 == status ) {
            status = performDecryption( sha1_ptr, desKey_ptr, fileSize );
        }

        HwUtil::deleteBuffer( desKey_ptr );
        HwUtil::deleteBuffer( sha1_ptr );

        //close input file
        closeInputFile();
    }
    
    return status;
}

/**
 * Checks the input file to make sure that it is a valid
 * DES file.
 *
 * @param fileSize              Size of the input file
 */
int DesDecDump::checkInputFile( long fileSize ) const {

    int status = 0;
    unsigned char remainder = 0;

    if( fileSize <= HwConstant::DES_HEADER_LENGTH ) {
        status = HwConstant::ERROR_INVALID_INPUT;
        HwUtil::setLastErrorMessage("Error: Invalid DES file. File size is less than DES header size");

    } else {
        fileSize -= HwConstant::DES_HEADER_LENGTH;
        remainder = fileSize % 8;
        
        if( 0 != remainder ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            HwUtil::setLastErrorMessage("Error: Invalid DES file. File size should be multiple of 8");
        }
    }
    
    return status;
}

/**
 * Gets the DES key
 * 
 * @param sha1_ptr              Sha-1 value
 * @param desKey_ptr            Des Key
 */
int DesDecDump::getKey( 
                    unsigned char *sha1_ptr, 
                    unsigned char *desKey_ptr ) const {

    int status = 0;

    status = fflush ( stdin );
    
    if( 0 == status ) {
        status = getPassword( sha1_ptr, false );

        if( 0 == status ) {
            status = getDesKey( sha1_ptr, desKey_ptr );
        }
    } else {
        HwUtil::setLastErrorMessage( "Unable to flush stdin before reading password" );
    }

    return status;
}

/**
 * Decrypts the data from input file
 *
 * @param sha1_ptr              Sha1 value of the pass-pharase
 * @param desKey_ptr            DES key
 * @param fileSize              Size of the input file
 */
int DesDecDump::performDecryption(
                    const unsigned char *sha1_ptr, 
                    const unsigned char *desKey_ptr,
                    long fileSize ) const {

    int status  = 0;
    int readLen = 0;
    int bytesRead = 0;
    bool headerProcessed = false;
    int numBytesToWrite = 0;
    int numPaddedBytes = 0;
    unsigned char numByteLastBlock = 0; //Number of bytes in the last 8-byte block of the original file
    Sha1Data sha1data;

    unsigned char *iv_ptr      = NULL;
    unsigned char *inBuf_ptr   = NULL;
    unsigned char *outBuf_ptr  = NULL;
    unsigned char *fileSha_ptr = NULL;

    DES_key_schedule desKeySched;

    status = HwUtil::createBuffer(HwConstant::DES_KEY_LENGTH, iv_ptr );
    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, inBuf_ptr );
    }
    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, outBuf_ptr );
    }
    if( 0 == status ) {
        status = HwUtil::createBuffer(SHA_DIGEST_LENGTH, fileSha_ptr );
    }
    if( 0 == status ) {
        status = getIv( sha1_ptr, iv_ptr );
    }
    if( 0 == status ) {
        status = getDesKeySchedule( desKey_ptr, &desKeySched );
    }

    if( 0 == status ) {
        
        while( (!feof(_fp)) && (0 == status) ) {
            //process the header first
            if( ! headerProcessed ) {
                status = processDesHeader( fileSha_ptr, numByteLastBlock );
                headerProcessed = true;
                bytesRead += HwConstant::DES_HEADER_LENGTH;

            } else {
                memset( inBuf_ptr, 0, HwConstant::BUFFER_SIZE );
                memset( outBuf_ptr, 0, HwConstant::BUFFER_SIZE );
            
                readLen = fread( inBuf_ptr, 
                                 sizeof(unsigned char), 
                                 HwConstant::BUFFER_SIZE,
                                 _fp );
            
                status = checkFileReadStatus();
                if( (0 == status) && (readLen > 0)  ) {
                
                    status = getDecryptedData( inBuf_ptr, outBuf_ptr, readLen, &desKeySched, iv_ptr ); 
                    if( 0 == status ) {
                        numBytesToWrite = readLen;

                        if( (readLen + bytesRead) >= fileSize ) {
                            //calculate the padded byte in the last block of 8 bytes block
                            //in the original file
                            numPaddedBytes = 8 - numByteLastBlock;
                            
                            //subtract the padded bytes as they weren't part of the original file
                            numBytesToWrite -= numPaddedBytes;
                        }

                        //calculate SHA-1 for the decrypted data
                        sha1data.update( outBuf_ptr, numBytesToWrite );
                        HwUtil::writeOutput( outBuf_ptr, numBytesToWrite );
                    }

                    bytesRead += readLen;
                }
            }
        }
            
        if( 0 == status ) {
            sha1data.final();
            status = compareSha1( fileSha_ptr, sha1data.getBuffer() );
        }
    }
        
    HwUtil::deleteBuffer(iv_ptr);
    HwUtil::deleteBuffer(inBuf_ptr);
    HwUtil::deleteBuffer(outBuf_ptr);
    HwUtil::deleteBuffer(fileSha_ptr);

    return status;
}

/**
 * Gets the decrypted data for the plain data.
 *
 * @param inBuf_ptr             Input buffer containing cipher text
 * @param outBuf_ptr            Output buffer
 * @param bufLen                Length of the buffer
 * @param keySchedule_ptr       Key Schedule
 * @param iv_ptr                Initialization vector
 */
int DesDecDump::getDecryptedData( 
        const unsigned char *inBuf_ptr,
        unsigned char *outBuf_ptr,
        int bufLen,
        DES_key_schedule *keySchedule_ptr,
        unsigned char *iv_ptr ) const {

    int status = 0;

    DES_ncbc_encrypt( 
            inBuf_ptr,
            outBuf_ptr,
            static_cast<long>(bufLen),
            keySchedule_ptr,
            (DES_cblock *)iv_ptr,
            0 );

    return status;
}

/**
 * Processes the DES header from the DES file
 * 
 * @param fileShaOut_ptr    Hash value of the original file
 * @param numByteLastBlock     Number of bytes padded towads the end of the file
 */
int DesDecDump::processDesHeader( 
        unsigned char *fileShaOut_ptr,
        unsigned char &numByteLastBlock ) const {

    int status = 0;
    int readLen = 0;
    std::string msg;
    unsigned char *buffer_ptr = NULL;

    status = HwUtil::createBuffer( HwConstant::DES_HEADER_LENGTH, buffer_ptr );

    if( 0 == status ) {
        readLen = fread( 
                    buffer_ptr, 
                    sizeof(unsigned char), 
                    HwConstant::DES_HEADER_LENGTH, 
                    _fp );

        status = checkFileReadStatus();
    }

    if( 0 == status ) {        
        if( HwConstant::DES_HEADER_LENGTH == readLen ) {
            status = verifyDesHeader(buffer_ptr, numByteLastBlock );

        } else {
            status = HwConstant::ERROR_INVALID_INPUT;
            msg = "Error: Invalid DES file. Header length is incorrect. (Expected = ";
            msg += HwUtil::getString( HwConstant::DES_HEADER_LENGTH );
            msg += " actual = ";
            msg += HwUtil::getString( readLen );
            msg += ")";

            HwUtil::setLastErrorMessage(msg);
        }
    }

    if( 0 == status ) {
        memcpy( fileShaOut_ptr, buffer_ptr + 4, SHA_DIGEST_LENGTH );
    }
    
    HwUtil::deleteBuffer( buffer_ptr );

    return status;
}

/**
 * Verifies the DES header
 * 
 * @param buffer_ptr            Buffer
 * @param numByteLastBlock      Number of bytes in the last 8-byte block of the original file
 */
int DesDecDump::verifyDesHeader( 
        const unsigned char *buffer_ptr,
        unsigned char &numByteLastBlock ) const {
    
    int status = 0;
    char des_array[4];
    std::string errorMsg;
    
    memcpy(des_array, buffer_ptr, 3 );
    des_array[3] = '\0';

    std::string strDes( des_array );

    if( HwConstant::DES_HEADER != strDes ) {
        status = HwConstant::ERROR_INVALID_INPUT;
        errorMsg = "Error: Invalid DES header. Header should start with ";
        errorMsg += HwConstant::DES_HEADER;
        errorMsg += ". Found ";
        errorMsg += strDes;
    }

    if( 0 == status ) {
        numByteLastBlock = buffer_ptr[3];
        
        if( ( numByteLastBlock <=0 ) || ( numByteLastBlock > 8 ) ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            errorMsg = "Error: Invalid DES header. Byte Padding should be between 1 and 8. Found ";
            errorMsg += HwUtil::getString( numByteLastBlock );
        }
    }

    if( 0 != status ) {
        HwUtil::setLastErrorMessage( errorMsg );
    }

    return status;
}

/**
 * Compares the Sha-1 value of original file and Sha-1 value of the
 * decryped file.
 * 
 * @param origSha1              Orignal Sha1 that is stored in the cipher file
 * @param newSha1               Sha1 that is computed for the deciphered text
 *
 * @return                      0 if comparison is successful, else non-zero
 */
int DesDecDump::compareSha1( 
        const unsigned char * origSha1,
        const unsigned char * newSha1 ) const {

    int status = 0;

    status = memcmp( origSha1, newSha1, SHA_DIGEST_LENGTH );

    if( 0 != status ) {
        std::string str = "\nError: Sha-1 value of the original file and Sha-1 value of the deciphered file does not match\n";
        str += "orig = ";
        str += HwUtil::getHexString( origSha1, SHA_DIGEST_LENGTH );
        str += "\nnew  = ";
        str += HwUtil::getHexString( newSha1, SHA_DIGEST_LENGTH );
        
        HwUtil::setLastErrorMessage( str );
    }

    return status;
}
