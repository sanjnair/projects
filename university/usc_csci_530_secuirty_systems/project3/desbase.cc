#include "desbase.h"
#include "hwconstant.h"
#include "hwutil.h"
#include "smartcardproxy.h"
#include "sha1data.h"

/**
 * C'tor.
 */
DesBase::DesBase() { 
    _fileSize = 0;
    _initialized = false;
    _fpFrom = NULL;
    _fpTo = NULL;
}

/**
 * Virtual D'tor.
 */
DesBase::~DesBase() {}

/**
 * Returns DES prompt string
 *
 * @param encryption            Encrption of decryption
 */
std::string DesBase::getDesPrompt( bool encrypt ) const {

    std::string str = "Enter passpharse for ";
    
    if( encrypt ) {
        str += "encryption";
    } else {
        str += "decryption";
    }
    
    str += " (max chars = ";
    str += HwUtil::getString(HwConstant::MAX_PASSWD_LENGTH );
    str += ")";
    str += ":";

    return str;
}

/**
 * Returns the sha1 value for the string
 *
 * @param str                   String for the sha1 hash to be computed
 * @param sha1_ptr              Sha-1 output
 */
int DesBase::getSha1FromStr( 
        const std::string & str, 
        unsigned char sha1Buf[SHA_DIGEST_LENGTH]  ) const {
    
    int status = 0;
    Sha1Data sha1data;

    if( str.length() > 0 ) {
        if( 0 == status ) {
            status = sha1data.update( (unsigned char *) str.c_str(), str.length() );
            
            if( 0 == status ) {
                status = sha1data.final();
            }
        }
        if( 0 == status ) {
            if( NULL != sha1data.getBuffer() ) {
                memcpy(sha1Buf, sha1data.getBuffer(), SHA_DIGEST_LENGTH ); 
            } else {
                status = -1;
                HwUtil::setLastErrorMessage("DesBase::getSha1FromStr: Invalid sha buffer"); 
            }
        }
        //SHA1( (unsigned char *) str.c_str(), str.length(), sha1Buf );
    } else {
        status = HwConstant::ERROR_INVALID_INPUT;
        HwUtil::setLastErrorMessage("Zero length password is not allowed");
    }

    return status;
}

/**
 * Gets the password from the user and returns the SHA-1 Hash
 * of the password.
 *
 * @param encrypt               If true, password will be verified.
 * @param password              Password
 * @param sha1Buf               Returns the Sha1 value of the password
 *
 * @return                      0 if OK, else non-zero value
 */
int DesBase::getPassword( 
                    bool encrypt,
                    std::string & password,
                    unsigned char sha1Buf[SHA_DIGEST_LENGTH] ) const {

    int status  = 0;
    int verify  = 0;
    char * buf  = NULL;
    char * buff = NULL;
    std::string msg; 


    std::string strPrompt;
    
    strPrompt = getDesPrompt( encrypt );
    verify = (encrypt) ? 1 : 0;
    
    status = HwUtil::createCharBuffer( HwConstant::MAX_PASSWD_LENGTH, buf );
    
    if( 0 == status ) {
        status = HwUtil::createCharBuffer( HwConstant::MAX_PASSWD_LENGTH, buff );
    }
    
    if( 0 == status ) {
        status = des_read_pw( 
                        buf, 
                        buff,
                        HwConstant::MAX_PASSWD_LENGTH, 
                        strPrompt.c_str(),
                        verify );
    }
   
    if( 0 != status ) {
        if( -2 == status ) { //ctrl-C
            HwUtil::setLastErrorMessage("User canceled the operation");
        } else {
            if( strlen(buff) <= 0 ) {
                msg  = "Password length should not be more than ";
                msg +=  HwUtil::getString(HwConstant::MAX_PASSWD_LENGTH );
                msg += " characters";
                HwUtil::setLastErrorMessage(msg);
            } else if( encrypt ) {
                msg = "Passwords does not match";
                HwUtil::setLastErrorMessage(msg);
            }
        }
    } else {
        password = buf;
        status = getSha1FromStr(password, sha1Buf );
    }
    
    HwUtil::deleteCharBuffer( buf );
    HwUtil::deleteCharBuffer( buff );

    return status;
}

/**
 * Intitializes the DES keys
 *
 * @param sha1Buf               Sha1 value
 * @param soPath                So Path
 */
int DesBase::initDesKeys( 
                unsigned char sha1Buf[SHA_DIGEST_LENGTH],
                const std::string & soPath ) {

    int status = 0;
    
    unsigned char key1[8];
    unsigned char key2[8];
    unsigned char key3[8];
    SmartCardProxy proxy;


    status = proxy.init( soPath );
    
    if( 0 == status ) {
        status = proxy.desKeyGen( sha1Buf, _iv, key1, key2, key3 );
    }

    if( 0 == status ) {
        DES_set_odd_parity( (DES_cblock*)key1 );
        DES_set_odd_parity( (DES_cblock*)key2 );
        DES_set_odd_parity( (DES_cblock*)key3 );
    }

    if( 0 == status ) {
        status = getDesKeySchedule( key1, &_ks1 );
    }
    if( 0 == status ) {
        status = getDesKeySchedule( key2, &_ks2);
    }
    if( 0 == status ) {
        status = getDesKeySchedule( key3, &_ks3 );
    }
    
    return status;
}

/**
 * Returns DES key schedule
 * 
 * @param key                   des key
 * @param ks                    key schedule 
 */
int DesBase::getDesKeySchedule( 
        const unsigned char key[8],
        DES_key_schedule *ks ) const {

    int status = 0;

    status = DES_set_key_checked( (DES_cblock*)key, ks );

    if( 0 != status ) {
        HwUtil::setLastErrorMessage("Unable to set the key schedule");
    }
    return status;
}

/**
 * Returns the file size. If error occurs, error is recorded.
 *
 * @param fileName              Name of the file
 * @param fileSize              Size of the file
 *
 */
int DesBase::getFileSize( 
                 const std::string & fileName,
                 long & fileSize ) const {
    
    int status = 0;

    status = HwUtil::getFileSize( fileName, fileSize );

    if( 0 == status ) {
        if( fileSize <= 0 ) {
            status = HwConstant::ERROR_FILE_READ;
            HwUtil::setLastErrorMessage("Input file length should be greater than 0");
        }
    }
    
    return status;
}

/**
 * Validates the init
 */
int DesBase::validateInit() const {
    
    int status = 0;

    if( ! _initialized ) {
        HwUtil::setLastErrorMessage("DesBase::validateInit: Not initialized");
        status = -1;
    }

    return status;
}
