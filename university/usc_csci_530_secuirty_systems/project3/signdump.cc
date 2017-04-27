#include "signdump.h"
#include "hwutil.h"
#include "hwconstant.h"
#include "sha1data.h"
#include "smartcardproxy.h"

#include <openssl/rsa.h>

/**
 * C'tor.
 *
 * @param filePath          File path
 */
SignDump::SignDump( const std::string filePath, const std::string soPath ) 
                : BaseDump( filePath, soPath ) {
}

/**
 * Virtual D'tor.
 */
SignDump::~SignDump() { }

/**
 * Dumps the output (based on input)
 */
int SignDump::dump() {

    int status = 0;
    std::string strSign;
    
    status = openInputFile( true );

    if( 0 == status ) {
        status = getSignedStr( strSign );
        
        if( 0 == status ) {
            HwUtil::writeOutputLn( strSign );
        }

        closeInputFile();
    }

    return status;
}

/**
 * Gets the signed SHA-1 signature for the file.
 *
 * @param strSign           Signed string
 */
int SignDump::getSignedStr( std::string & strSign ) const {
    int status = 0;
    int sigSize = 0;
    SmartCardProxy proxy;
    unsigned char sha1_buf[SHA_DIGEST_LENGTH];
    unsigned char * buf_return = NULL;
    
    Sha1Data shaData;
    status = shaData.calculate( _fp );

    if( 0 == status ) {
        memcpy( sha1_buf, shaData.getBuffer(), SHA_DIGEST_LENGTH );
        status = proxy.init( _soPath );
    }
    
    if( 0 == status ) {
        status = proxy.getSignatureSize( sigSize );

        if( 0 == status ) {
            status = HwUtil::createBuffer( sigSize, buf_return );
        }
    
        if( 0 == status ) {
            status = proxy.sign( sha1_buf, buf_return );
        }

        if( 0 == status ) {
            strSign = HwUtil::getHexString( buf_return, sigSize );
        }

    
        int tempStatus = proxy.cleanup();
        if( 0 == status ) {
            status = tempStatus;
        }
    }

    HwUtil::deleteBuffer( buf_return );

    return status;
}

