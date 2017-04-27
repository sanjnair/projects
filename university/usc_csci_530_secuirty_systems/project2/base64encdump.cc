#include "base64encdump.h"
#include "hwconstant.h"
#include "hwutil.h"
#include <openssl/err.h>
#include <openssl/evp.h>

/**
 * C'tor.
 *
 * @param filePath          File path
 */
Base64EncDump::Base64EncDump(std::string filePath) : BaseDump(filePath) {
}

/**
 * Virtual D'tor.
 */
Base64EncDump::~Base64EncDump() {}

/**
 * Dumps the output (based on input file or stdin)
 */
int Base64EncDump::dump() {

    int status                  = 0;
    int readLen                 = 0;
    unsigned char *buffer_ptr   = NULL;
    BIO * bio_ptr               = NULL;
    
    status = openInputFile();

    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, buffer_ptr );
        
        if( 0 == status ) {
            status = createBIO( bio_ptr );
        
            if( 0 == status ) {
                while( (!feof(_fp)) && (0 == status) ) {
                    memset(buffer_ptr, 0, HwConstant::BUFFER_SIZE );
                    readLen = fread( buffer_ptr, 
                                     sizeof(unsigned char), 
                                     HwConstant::BUFFER_SIZE,
                                     _fp );

                    status = checkFileReadStatus();

                    if( (0 == status) && (readLen > 0)  ) {
                        BIO_write( bio_ptr, buffer_ptr, readLen );
                    }
                }

                if( 0 == status ) {
                    BIO_flush( bio_ptr );
                }
            }
        }

        deleteBIO( bio_ptr );
        HwUtil::deleteBuffer( buffer_ptr );

        //close input file
        closeInputFile();
    }
    
    return status;
}

/**
 * Creates and initializes BIO variables
 *
 * @return      returns 0 if creation is successful
 */
int Base64EncDump::createBIO( BIO *& bio ) const {

    int status = HwConstant::ERROR_OUT_OF_MEMORY;
    BIO * b64_ptr = NULL;    

    bio = BIO_new_fp( stdout, BIO_NOCLOSE );

    if( NULL != bio ) {
        b64_ptr = BIO_new( BIO_f_base64() );

        if( NULL != b64_ptr ) {
            bio = BIO_push( b64_ptr, bio );
            BIO_set_write_buf_size( bio, HwConstant::BUFFER_SIZE ); 
            status = 0;
        }
    }
 
    if( 0 != status) {
        HwUtil::setLastErrorMessage("Unable to create BIO");
        deleteBIO( bio );        
    }

    return status;
}

/**
 * Deletes BIO
 *
 * @return      returns 0 if deletion is successful
 */
void Base64EncDump::deleteBIO( BIO *& bio ) const {
    
    if( NULL != bio ) {
        BIO_free_all( bio );
        bio = NULL;
    }
}


