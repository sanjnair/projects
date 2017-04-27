#include "secretdump.h"
#include "hwutil.h"
#include "hwconstant.h"

#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/sha.h>


//-----------------------------------------------------------
//declare the callback funciton for Key Generation
void keyGen_callBack( int status, int iteration, void * cb_arg);
//-----------------------------------------------------------

/**
 * C'tor.
 *
 * @param filePath          File path
 */
SecretDump::SecretDump( const std::string filePath, const std::string soPath ) 
                : BaseDump( filePath, soPath ) {
}

/**
 * Virtual D'tor.
 */
SecretDump::~SecretDump() { }

/**
 * Dumps the output (based on input file or stdin)
 */
int SecretDump::dump() {

    int status = 0;
    RSA * rsa_ptr = NULL;
    char * cb_arg = ".";

    rsa_ptr = RSA_generate_key( 
                HwConstant::RSA_PUBLIC_MODULUS_SIZE,
                RSA_F4,
                &keyGen_callBack,
                cb_arg );
    
    status = static_cast<int>( ERR_get_error() );

    if( (NULL == rsa_ptr) || ( 0 != status ) ) {
        std::string msg = "Unable to generate RSA key. ";

        if( 0 != status ) {
            msg += getOpenSslErrStr( ERR_get_error() );                
        } else {
            status = HwConstant::ERROR_INVALID_INPUT;
        }
    } 

    if( 0 == status ) {
        std::string strCode;
        status = getRsaCode( rsa_ptr, strCode );
    }

    if( NULL != rsa_ptr ) {
        RSA_free( rsa_ptr );
        rsa_ptr = NULL;
    }

    return status;
}


/**
 * Generate the code for the RSA
 *
 * @param rsa_ptr           RSA pointer
 * @param strCode           code returned
 */
int SecretDump::getRsaCode( 
            const RSA *rsa_ptr,
            std::string & strCode ) const {

    int status = 0;
    std::string strTemp;
    
    strCode = "#ifndef SHA_DIGEST_LENGTH\n#include <openssl/sha.h>\n#endif\n\n";
    
    status = getArrayCode( rsa_ptr->d, HwConstant::RSA_PRIVATE_KEY, true, sizeof(BN_ULONG), strTemp );

    if( 0 == status ) {
        strCode += strTemp;

        status = getArrayCode( rsa_ptr->e, HwConstant::RSA_PUBLIC_EXPONENT, true, 3, strTemp );
    }

    if( 0 == status ) {
        strCode += strTemp;

        status = getArrayCode( rsa_ptr->n, HwConstant::RSA_PUBLIC_MODULUS, true, sizeof(BN_ULONG), strTemp );
    }

    if( 0 == status ) {
        strCode += strTemp;

        status = getArrayCodeForRandom( HwConstant::RSA_RANDOM_BITS_1, strTemp );
    }

    if( 0 == status ) {
        strCode += strTemp;

        status = getArrayCodeForRandom( HwConstant::RSA_RANDOM_BITS_2, strTemp );
    }

    if( 0 == status ) {
        strCode += strTemp;
        HwUtil::writeOutputLn( strCode );
    }

    return status;
}

/**
 * Get the aray code for byte array
 *
 * @param buffer            Byte buffer
 * @param bufferLen         Length of the buffer
 * @param name              Name of the array
 * @param constName         Name of the constant
 * @param strCode           code returned
 *
 */
int SecretDump::getArrayCode(
        const unsigned char * buffer,
        int bufferLen,
        const std::string & name,
        bool genSizeConst,
        std::string & strCode ) const {

    int status = 0;

    strCode  = "static unsigned char HW4_";
    strCode += name;
    strCode += "[";

    if( ! genSizeConst ) {
        strCode += "SHA_DIGEST_LENGTH";
    }
    strCode += "] = {\n  ";
        
    for( int i=0; i<bufferLen; i++ ) {
        bool isLastIter = (i == (bufferLen - 1) );

        if( (i>0) && (0 == (i % 8)) ) {
            if( ! isLastIter ) {
                strCode += "\n  ";
            }
        }
        
        strCode += "0x";
        strCode += HwUtil::getHexString( buffer[i] );
        
        if( ! isLastIter ) {
            strCode += ",";
        }
    }
    
    strCode += "\n};";
    
    if( genSizeConst ) {
        strCode += "\n";
        strCode += "static int HW4_";
        strCode += name;
        strCode += "_size = ";
        strCode += HwUtil::getString(bufferLen);
        strCode += ";";
    } 

    strCode += "\n\n";

    return status;
}

/**
 * Gets the Array code for BIGNUM.
 *
 * @param bigNum_ptr        BIGNUM pointer
 * @param name              Name of the array
 * @param genSizeConst      To generate size constant
 * @param lastNumBytes      Number of bytes to be considered in the last number
 * @param strCode           code returned
 */
int SecretDump::getArrayCode( 
                    const BIGNUM * bigNum_ptr,
                    const std::string & name,
                    bool genSizeConst,
                    int lastNumBytes,
                    std::string & strCode ) const {

    
    assert( (lastNumBytes >0) && (lastNumBytes <= 4) );

    int status = 0;
    int bufferLen = 0;
    int retLen = 0;
    unsigned char * buffer = NULL;    
    
    //int dataSize = sizeof(BN_ULONG);
    
    if( (NULL != bigNum_ptr) && ( bigNum_ptr->top > 0 ) ) {
        
        bufferLen = BN_num_bytes(bigNum_ptr);
        HwUtil::createBuffer( bufferLen, buffer );

        retLen = BN_bn2bin(bigNum_ptr, buffer);

        if( retLen != bufferLen  ) {
            std::string msg = "BN_bn2bin did not return correct bytes. (bufferLen = ";
            msg += HwUtil::getString( bufferLen );
            msg += ", retLen = ";
            msg += HwUtil::getString( retLen );
            msg += " )";

            HwUtil::setLastErrorMessage( msg );
            status = HwConstant::ERROR_INVALID_INPUT;
        }
        
        if( 0 == status ) {
            status = getArrayCode( buffer, bufferLen, name, genSizeConst, strCode );
        }

    } else {
        std::string msg = "Invalid BIGNUM for array " + name;
        HwUtil::setLastErrorMessage( msg );
        status = HwConstant::ERROR_INVALID_INPUT;
    }

    HwUtil::deleteBuffer( buffer );

    return status;
}

/**
 * Gets the array code for the random numbers
 * 
 * @param name              Name of the array
 * @param strCode           code returned
 */
int SecretDump::getArrayCodeForRandom( 
        const std::string & name,
        std::string & strCode ) const {

    int status = 0;
    unsigned char * buffer = NULL;

    status = HwUtil::createBuffer( SHA_DIGEST_LENGTH, buffer );

    if( 0 == status ) {
        status = genRandomBytes( buffer, SHA_DIGEST_LENGTH );
    }

    if( 0 == status ) {
        status = getArrayCode( buffer, SHA_DIGEST_LENGTH, name, false, strCode );
    }

    HwUtil::deleteBuffer( buffer );
    
    return status;
}

/**
 * generates the random bytes
 *
 * @param buffer            Buffer to hold data
 * @param bufferLen         Lengh of the buffer
 */
int SecretDump::genRandomBytes (
        unsigned char * buffer,
        int bufferLen ) const {

    int status = 0;
    std::string strError;
    
    status = RAND_bytes( buffer, bufferLen );

    if( 1 == status ) {
        status = 0;
    } else {
        strError = "Unable to generate random numbers. ";
        strError += getOpenSslErrStr( ERR_get_error() );
    }

    return status;
}

//------------------------------------------------------------------------------------
/** 
 * Callback from RSA public key generation
 *
 */
void keyGen_callBack( 
        int status, 
        int iteration, 
        void * cb_arg) {

    std::string str;
    
    str = (char*) cb_arg;
    HwUtil::writeToFile( stderr, str );

    //When the n-th randomly generated prime is rejected as not suitable for the key, 
    //callback(2, n, cb_arg) is called. 
    if( 2 == status ) {
        str = "Randomly generated prime is not suitable for the key";
        HwUtil::writeError( str, false );
    }

    //When a random p has been found with p-1 relatively prime to e, 
    //it is called as callback(3, 0, cb_arg). 
    if( (3 == status) && (0 == iteration) ) {
        HwUtil::writeError( "+", false );
    }

    //The process is then repeated for prime q with callback(3, 1, cb_arg). 
    if( (3 == status) && (1 == iteration) ) {
        HwUtil::writeError( "");
        fflush( stderr );
    }
}


