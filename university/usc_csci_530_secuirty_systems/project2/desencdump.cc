#include "desencdump.h"
#include "hwconstant.h"
#include "hwutil.h"
#include "sha1data.h"

/**
 * C'tor.
 *
 * @param filePath          File path
 */
DesEncDump::DesEncDump(std::string filePath) : DesBaseDump(filePath) {
    
}

/**
 * Virtual D'tor.
 */
DesEncDump::~DesEncDump() {}

/**
 * Dumps the output (based on input file or stdin)
 */
int DesEncDump::dump() {

    int status                  = 0;
    unsigned char *sha1_ptr     = NULL;
    unsigned char *desKey_ptr   = NULL; //typedef unsigned char DES_cblock[8];

    status = openInputFile();

    if( 0 == status ) {
        status = HwUtil::createBuffer( SHA_DIGEST_LENGTH, sha1_ptr );

        if( 0 == status ) {
            status = HwUtil::createBuffer( HwConstant::DES_KEY_LENGTH, desKey_ptr );
        }

        if( 0 == status ) {
            status = getKey( sha1_ptr, desKey_ptr );
        }

        if( 0 == status ) {
            status = performEncryption( sha1_ptr, desKey_ptr );
        }

        HwUtil::deleteBuffer( desKey_ptr );
        HwUtil::deleteBuffer( sha1_ptr );
       
        //close input file
        closeInputFile();
    }
    
    return status;
}

/**
 * Gets the DES key
 * 
 * @param sha1_ptr              Sha-1 value
 * @param desKey_ptr            Des Key
 */
int DesEncDump::getKey( 
                    unsigned char *sha1_ptr, 
                    unsigned char *desKey_ptr ) const {

    int status = 0;
    bool bContinue = false;

    do {
        
        status = fflush ( stdin );

        if( 0 == status ) {
            if( 0 == status ) {
                status = getPassword( sha1_ptr, true );
            }
        
            if( 0 == status ) {
                status = getDesKey( sha1_ptr, desKey_ptr );
            }

            if( 0 == status ) {
                if( DES_is_weak_key( (des_cblock *) desKey_ptr ) ) {
                    HwUtil::writeOutput( "Error: Pass phrase produced weak key. Do you want to retry(y)" );
                    status = fflush ( stdin );
                    if( 0 == status ) {
                        char inChar;
                        inChar = static_cast<char>( getchar() );
                        inChar = toupper( inChar );
            
                        if( 'Y' == inChar ) {
                            bContinue = true;
                        }

                    }
                }
            }
        } else {
            HwUtil::setLastErrorMessage( "Unable to flush stdin before reading password" );
        }

    } while( bContinue );

    return status;
}

/**
 * Encrypts the data from input file
 */
int DesEncDump::performEncryption(
                    const unsigned char *sha1_ptr, 
                    const unsigned char *desKey_ptr ) const {

    int status  = 0;
    int readLen = 0;
    unsigned char *iv_ptr      = NULL;
    unsigned char *inBuf_ptr   = NULL;
    unsigned char *outBuf_ptr  = NULL;
    DES_key_schedule desKeySched;

    status = HwUtil::createBuffer(HwConstant::DES_KEY_LENGTH, iv_ptr );
    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, inBuf_ptr );
    }

    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, outBuf_ptr );
    }

    if( 0 == status ) {
        status = getIv( sha1_ptr, iv_ptr );
    }

    if( 0 == status ) {
        status = getDesKeySchedule( desKey_ptr, &desKeySched );
    }

    if( 0 == status ) {
        writeEncryptionHeader();
        
        int remainder = 0;

        while( (!feof(_fp)) && (0 == status) ) {
            memset( inBuf_ptr, 0, HwConstant::BUFFER_SIZE );
            memset( outBuf_ptr, 0, HwConstant::BUFFER_SIZE );
            
            readLen = fread( inBuf_ptr, 
                             sizeof(unsigned char), 
                             HwConstant::BUFFER_SIZE,
                             _fp );

            status = checkFileReadStatus();
            
            if( (0 == status) && (readLen > 0)  ) {

                //if readlen is not a multiple of 8, we have to pad 
                //the input buffer with 0's until the next 8 byte boundary
                remainder = readLen % 8;

                if( 0 != remainder ) {
                    readLen += ( 8 - remainder );
                }

                status = getEncryptedData( inBuf_ptr, outBuf_ptr, readLen, &desKeySched, iv_ptr ); 

                if( 0 == status ) {
                    HwUtil::writeOutput( outBuf_ptr, readLen );
                }
            }
        }
    }
    
    HwUtil::deleteBuffer(iv_ptr);
    HwUtil::deleteBuffer(inBuf_ptr);
    HwUtil::deleteBuffer(outBuf_ptr);

    return status;
}

/**
 * Writes the DES encryption header
 *
 */
int DesEncDump::writeEncryptionHeader() const {

    int status = 0;
    long fileSize = 0;
    unsigned char remainder = 0;
    Sha1Data sha1data;
    
    status = getFileSize( fileSize );

    if( 0 == status ) {
        if( fileSize > 0 ) {
        
            //determine how many bytes are in the last block of the file
            remainder = fileSize % 8;     

            if( 0 == remainder ) {
                remainder = 8;
            }
            
            if( 0 == status ) {
                status = sha1data.calculate( _fp );
            }

            if( 0 == status ) {
                //set the file pointer at the begining
                status = fseek( _fp, 0, SEEK_SET );

                if( 0 != status ) {
                    HwUtil::setLastErrorMessage("Error: Unable to seek the file pointer to the begining of the file");
                }
            }

      
            if( 0 == status ) {
                unsigned char remainBuff[1];
                remainBuff[0] = remainder;
                
                HwUtil::writeOutput( std::string(HwConstant::DES_HEADER) );
                HwUtil::writeOutput( remainBuff, 1 );
                HwUtil::writeOutput( sha1data.getBuffer(), SHA_DIGEST_LENGTH );
            }

        } else {
            status = HwConstant::ERROR_FILE_READ;
            HwUtil::setLastErrorMessage("Error: Input file length should be greater than 0");
        }   
    }

    return status;
}

/**
 * Gets the encrypted data for the plain data.
 *
 * @param inBuf_ptr             Input buffer containing plain text
 * @param outBuf_ptr            Output buffer
 * @param bufLen                Length of the buffer
 * @param keySchedule_ptr       Key Schedule
 * @param iv_ptr                Initialization vector
 */
int DesEncDump::getEncryptedData( 
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
            1 );

    return status;
}
