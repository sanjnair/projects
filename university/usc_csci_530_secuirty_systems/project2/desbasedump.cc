#include "desbasedump.h"
#include "hwutil.h"
#include "hwconstant.h"

/**
 * C'tor.
 *
 * @param filePath          File path
 */
DesBaseDump::DesBaseDump(std::string filePath) : BaseDump( filePath ) {
}

/**
 * Virtual D'tor.
 */
DesBaseDump::~DesBaseDump() {}

/**
 * Overloaded method to ensure that the input file
 * exists in the file system
 * 
 */
int DesBaseDump::openInputFile() {

    int status = 0;
    std::string errorMsg;

    if( _filePath.length() > 0 ) {
        _fp = fopen(_filePath.c_str(), "rb");

        if( NULL == _fp ) {
            errorMsg =  "Error: Unable to open input file \"";
            errorMsg += _filePath;
            errorMsg += "\" for reading.";
        }
    } else {
        errorMsg = "Error: a valid filename is needed to perform this operation";
    }

    if( errorMsg.length() > 0 ) {
        HwUtil::setLastErrorMessage(errorMsg);
        status = HwConstant::ERROR_FILE_OPEN;
    }
       
    return status;
}

/**
 * Returns the file size. If error occurs, error is recorded.
 *
 */
int DesBaseDump::getFileSize( long & fileSize ) const {
    int status = 0;

    status = HwUtil::getFileSize( _filePath, fileSize );

    if( 0 == status ) {
        if( fileSize <= 0 ) {
            status = HwConstant::ERROR_FILE_READ;
            HwUtil::setLastErrorMessage("Error: Input file length should be greater than 0");
        }
    }
    
    return status;
}


/**
 * Gets the password from the user and returns the SHA-1 Hash
 * of the password.
 *
 * @param sha1_ptr              Returns the Sha1 value of the password
 * @param encrypt               If true, password will be verified.
 *
 * @return                      0 if OK, else non-zero value
 */
int DesBaseDump::getPassword( 
                    unsigned char * sha1_ptr,
                    bool encrypt ) const {

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
        if( -2 != status ) { //ctrl-C
            if( strlen(buff) <= 0 ) {
                msg  = "Error: Password length should not be more than ";
                msg +=  HwUtil::getString(HwConstant::MAX_PASSWD_LENGTH );
                msg += " characters";
                HwUtil::setLastErrorMessage(msg);
            } else if( encrypt ) {
                msg = "Error: Passwords does not match";
                HwUtil::setLastErrorMessage(msg);
            }
        }
    } else {
        msg = buf;
        status = getSha1FromStr(msg, sha1_ptr );
    }
    
    HwUtil::deleteCharBuffer( buf );
    HwUtil::deleteCharBuffer( buff );

    return status;
}

/**
 * Returns DES prompt string
 *
 * @param encryption            Encrption of decryption
 */
std::string DesBaseDump::getDesPrompt( bool encrypt ) const {

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
int DesBaseDump::getSha1FromStr( 
        const std::string & str, 
        unsigned char * sha1_ptr ) const {
    
    int status = 0;

    if( str.length() > 0 ) {
        
        if( 0 == status ) {
            SHA1( (unsigned char *) str.c_str(), str.length(), sha1_ptr );
        }

    } else {
        status = HwConstant::ERROR_INVALID_INPUT;
        HwUtil::setLastErrorMessage("Error: Zero length password is not allowed");
    }

    return status;
}

/**
 * Gets the DES key from the sha-1 value
 *
 * @param sha1_ptr              Sha-1 value
 * @param desKey_ptr            DES key block
 */
int DesBaseDump::getDesKey( const unsigned char *sha1_ptr, unsigned char *desKey_ptr ) const{

    int status = 0;
    

    //leading 8-bytes of the sha-1 value is the key
    for( int i=0; i < HwConstant::DES_KEY_LENGTH; i++ ) {
        desKey_ptr[i] = sha1_ptr[i];
    }
    
    //set odd parity
    DES_set_odd_parity( (des_cblock *)desKey_ptr );
    
    return status;
}

/**
 * Returns the initialization vector for the DES encryption
 * 
 * @param sha1_ptr              Sha1 value
 * @param iv_ptr                Intitialization vector 
 */
int DesBaseDump::getIv( 
        const unsigned char * sha1_ptr, 
        unsigned char * iv_ptr ) const {

    int status = 0;

    //first 8 byte of the sha-1 is used as the des key. 
    //Next 8 byte will be used for initialization vector
    memcpy(iv_ptr, sha1_ptr + HwConstant::DES_KEY_LENGTH, HwConstant::DES_KEY_LENGTH );

    return status;
}

/**
 * Returns DES key schedule
 * 
 * @param desKey_ptr            des key
 * @param desKeySched_out_ptr   DES key schedule 
 */
int DesBaseDump::getDesKeySchedule( 
        const unsigned char * desKey_ptr,
        DES_key_schedule * desKeySched_out_ptr ) const {

    int status = 0;

    status = DES_set_key_checked( (DES_cblock *)desKey_ptr, desKeySched_out_ptr );

    if( 0 != status ) {
        HwUtil::setLastErrorMessage("Unable to set the key schedule");
    }
    return status;
}


