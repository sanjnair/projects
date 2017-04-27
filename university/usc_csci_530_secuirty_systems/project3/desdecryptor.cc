#include "desdecryptor.h"
#include "hwconstant.h"
#include "hwutil.h"
#include "sha1data.h"

/**
 * C'tor.
 */
DesDecryptor::DesDecryptor() : DesBase() {

}

/**
 * Virtual D'tor.
 */
DesDecryptor::~DesDecryptor() { }

/**
 * Intializes the des
 *
 * @param soPath            SoPath
 * @param filePath          File Path
 */
int DesDecryptor::init( 
        const std::string & soPath,
        const std::string & filePath )  {

    int status = 0;
    unsigned char sha1Buf[SHA_DIGEST_LENGTH];

    if( 0 == status ) {
        status = getFileSize( filePath, _fileSize );
    }

    if( 0 == status ) {
        status = checkInputFile( _fileSize );
    }

    if( 0 == status ) {
        status = getPassword( false, _pwd, sha1Buf );
    }

    if( 0 == status ) {
        status = initDesKeys( sha1Buf, soPath );
    }
    
    if( 0 == status ) {
        _initialized = true;
    }

    return status;
}

/**
 * Returns the password
 */
std::string DesDecryptor::getPwd() const {
    return _pwd;
}

/**
 * decrypts the data and write to fp
 *
 * @param _fpFrom            From file pointer
 * @param fpTo              To File pointer
 */
int DesDecryptor::decrypt(
        FILE * fpFrom,
        FILE * fpTo ) {

    int status = 0;

    _fpFrom = fpFrom;
    _fpTo = fpTo;

    status = decrypt();

    return status;
}

/**
 * decrypts the data and write to fp
 *
 * @param fileFrom            From file
 * @param fileTo              To File
 */
int DesDecryptor::decrypt(
        const std::string & fileFrom,
        const std::string & fileTo ) {

    int status = 0;

    _fileFrom = fileFrom;
    _fileTo = fileTo;

    status = decrypt();
    
    if( NULL != _fpFrom ) {
        HwUtil::closeFile( _fpFrom );
        _fpFrom = NULL;
        _fileFrom = "";
    }
    if( NULL != _fpTo ) {
        HwUtil::closeFile( _fpTo );
        _fpTo = NULL;
        _fileTo = "";
    }
    
    return status;
}


/**
 * decrypts the data and write to fp
 */
int DesDecryptor::decrypt() {

    int status = 0;
    
    bool headerProcessed    = false;
    long readLen            = 0;
    int bytesRead           = 0;
    int shaRead             = 0;
    int size                = 0;
    int numBytesToWrite     = 0;
    int numPaddedBytes      = 0;
    
    unsigned char *inBuf    = NULL;
    unsigned char *outBuf   = NULL;
    
    unsigned char numByteLastBlock = 0;             //Number of bytes in the last 8-byte block of the original file
    unsigned char fileSha[SHA_DIGEST_LENGTH];       //sha1 value of the original data
    Sha1Data sha1data;

    //--------------------------------------------------
    
    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, inBuf );
    }
    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, outBuf );
    }

    if( (0 == status) && (NULL == _fpFrom) ) {
        status = HwUtil::openFile( _fileFrom, "rb", _fpFrom );
    }

    //open the output file if not already open
    if( (0 == status) && (NULL == _fpTo ) ) {
        status = HwUtil::openFile( _fileTo, "wb", _fpTo );
    }

    if( 0 == status ) {
        while( (0 == status) && (!feof(_fpFrom)) ) {
            //process the header first
            if( ! headerProcessed ) {
                status = processDesHeader( _fpFrom, numByteLastBlock );
                headerProcessed = true;
                bytesRead += HwConstant::DES_HEADER_LENGTH;

            } else {
                memset( inBuf, 0, HwConstant::BUFFER_SIZE );
                memset( outBuf, 0, HwConstant::BUFFER_SIZE );
                
                readLen = fread( inBuf, sizeof(unsigned char), HwConstant::BUFFER_SIZE, _fpFrom );
                status = HwUtil::checkFileReadStatus( _fpFrom );

                if( (0 == status) && (readLen > 0)  ) {
                    bytesRead += readLen;

                    if( 0 == shaRead ) {
                       size = _fileSize - bytesRead;
                       
                       if( 0 == size ) { //that is the entire sha is inside the current buffer
                            size = SHA_DIGEST_LENGTH;   
                       }
                        
                       if( size <= SHA_DIGEST_LENGTH ) {
                            shaRead = size;
                            memcpy( fileSha, inBuf + (readLen - shaRead), shaRead );
                            readLen -= shaRead; //adjust the readlen based on 
                        }
                    } else {
                        size = readLen;
                        if( SHA_DIGEST_LENGTH != (shaRead + size) ) {
                            std::string msg(" Invalid file. Unable to get the sha value shaRead = ");
                            msg += HwUtil::getString( shaRead );
                            msg += " size = ";
                            msg += HwUtil::getString( size );

                            status = HwConstant::ERROR_INVALID_INPUT;
                            HwUtil::setLastErrorMessage(msg);
                        } else {
                            memcpy( fileSha + shaRead, inBuf, readLen );
                        }
                    }
                }

                if( (0 == status) && (readLen > 0)  ) {
                    DES_ede3_cbc_encrypt( inBuf, outBuf, readLen, &_ks1,&_ks2, &_ks3, (DES_cblock *)_iv, 0);

                    if( (bytesRead + SHA_DIGEST_LENGTH) >= _fileSize ) {
                        //calculate the padded byte in the last block of 8 bytes block
                        //in the original file
                        numPaddedBytes = 8 - numByteLastBlock;
                        
                        //subtract the padded bytes as they weren't part of the original file
                        numBytesToWrite = readLen - numPaddedBytes;
                    } else {
                        numBytesToWrite = readLen;
                    }
                    
                    //calculate SHA-1 for the decrypted data
                    status = sha1data.update( outBuf, numBytesToWrite );

                    if( 0 == status ) {
                        status = HwUtil::writeToFile( _fpTo, outBuf, numBytesToWrite );
                    }
                }
            }
        } //end while
            
        if( 0 == status ) {
            sha1data.final();
            status = compareSha1( fileSha, sha1data.getBuffer() );
        }
    }
        
    HwUtil::deleteBuffer(inBuf);
    HwUtil::deleteBuffer(outBuf);


    return status;
}

/**
 * Checks the input file to make sure that it is a valid
 * DES file.
 *
 * @param fileSize              Size of the input file
 */
int DesDecryptor::checkInputFile( long fileSize ) const {

    int status = 0;
    unsigned char remainder = 0;

    if( fileSize <= HwConstant::DES_HEADER_LENGTH ) {
        status = HwConstant::ERROR_INVALID_INPUT;
        HwUtil::setLastErrorMessage("Invalid DES file. File size is less than DES header size");

    } else {
        fileSize -= ( HwConstant::DES_HEADER_LENGTH + SHA_DIGEST_LENGTH );
        remainder = fileSize % 8;
        
        if( 0 != remainder ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            HwUtil::setLastErrorMessage("Invalid DES file. File size should be multiple of 8");
        }
    }
    
    return status;
}

/**
 * Processes the DES header from the DES file
 * 
 * @param fp                    File pointer
 * @param numByteLastBlock      Number of bytes padded towads the end of the file
 */
int DesDecryptor::processDesHeader( 
            FILE * fp,
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
                    fp );

        status = HwUtil::checkFileReadStatus( fp );
    }

    if( 0 == status ) {        
        if( HwConstant::DES_HEADER_LENGTH == readLen ) {
            status = verifyDesHeader(buffer_ptr, numByteLastBlock );

        } else {
            status = HwConstant::ERROR_INVALID_INPUT;
            msg = "Invalid DES file. Header length is incorrect. (Expected = ";
            msg += HwUtil::getString( HwConstant::DES_HEADER_LENGTH );
            msg += " actual = ";
            msg += HwUtil::getString( readLen );
            msg += ")";

            HwUtil::setLastErrorMessage(msg);
        }
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
int DesDecryptor::verifyDesHeader( 
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
        errorMsg = "Invalid DES header. Header should start with ";
        errorMsg += HwConstant::DES_HEADER;
        errorMsg += ". Found ";
        errorMsg += strDes;
    }

    if( 0 == status ) {
        numByteLastBlock = buffer_ptr[3];
        
        if( ( numByteLastBlock <=0 ) || ( numByteLastBlock > 8 ) ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            errorMsg = "Invalid DES header. Byte Padding should be between 1 and 8. Found ";
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
int DesDecryptor::compareSha1( 
        const unsigned char * origSha1,
        const unsigned char * newSha1 ) const {

    int status = 0;

    if( (NULL != origSha1) && (NULL != newSha1) ) {
    
        status = memcmp( origSha1, newSha1, SHA_DIGEST_LENGTH );

        if( 0 != status ) {
            std::string str = "\nSha-1 value of the original file and Sha-1 value of the deciphered file does not match\n";
            str += "orig = ";
            str += HwUtil::getHexString( origSha1, SHA_DIGEST_LENGTH );
            str += "\nnew  = ";
            str += HwUtil::getHexString( newSha1, SHA_DIGEST_LENGTH );
        
            HwUtil::setLastErrorMessage( str );
        }
    } else {
        status = HwConstant::ERROR_INVALID_INPUT;
        HwUtil::setLastErrorMessage( "DesDecryptor::compareSha1: Input sha buffers are null" );
    }

    return status;
}
