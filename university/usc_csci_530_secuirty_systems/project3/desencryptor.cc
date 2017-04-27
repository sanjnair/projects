#include "desencryptor.h"
#include "hwconstant.h"
#include "hwutil.h"
#include "sha1data.h"

/**
 * C'tor.
 */
DesEncryptor::DesEncryptor() : DesBase() {
}

/**
 * Virtual D'tor.
 */
DesEncryptor::~DesEncryptor() { }

/**
 * Intializes the des
 *
 * @param soPath            SoPath
 * @param filePath          File Path
 */
int DesEncryptor::init( 
        const std::string & soPath,
        const std::string & filePath )  {

    int status = 0;
    unsigned char sha1Buf[SHA_DIGEST_LENGTH];

    if( 0 == status ) {
        status = getFileSize( filePath, _fileSize );
    }

    if( 0 == status) {
        if( _pwd.length() <= 0 ) {
            status = getPassword( true, _pwd, sha1Buf );
        } else {
            status = getSha1FromStr(_pwd, sha1Buf );
        }
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
 * Sets the password
 *
 * @param password
 */
void DesEncryptor::setPwd( const std::string & password ) {
    _pwd = password;
}

/**
 * encrypts the data and write to fp
 *
 * @param fpFrom            From file pointer
 * @param fpTo              To File pointer
 */
int DesEncryptor::encrypt(
        FILE * fpFrom,
        FILE * fpTo ) {

    int status = 0;

    _fpFrom = fpFrom;
    _fpTo = fpTo;

    status = encrypt();

    return status;
}

/**
 * encrypts the data and write to fp
 *
 * @param fileFrom            From file
 * @param fileTo              To File
 */
int DesEncryptor::encrypt(
        const std::string & fileFrom,
        const std::string & fileTo ) {

    int status = 0;

    _fileFrom = fileFrom;
    _fileTo = fileTo;

    status = encrypt();
    
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
 * encrypts the data and write to fp
 */
int DesEncryptor::encrypt() {

    int status = 0;
    long readLen = 0;
    int remainder = 0;
    bool headerWritten = false;
    Sha1Data shaData;
    unsigned char *inBuf  = NULL;
    unsigned char *outBuf = NULL;

    status = validateInit();
    
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

    
    while( (0 == status) && (!feof( _fpFrom)) ) {
    
        memset( inBuf, 0, HwConstant::BUFFER_SIZE );
        memset( outBuf, 0, HwConstant::BUFFER_SIZE );
        
        readLen = fread( inBuf, sizeof(unsigned char), HwConstant::BUFFER_SIZE, _fpFrom );
        status = HwUtil::checkFileReadStatus( _fpFrom );
        
        if( (0 == status) && (readLen > 0)  ) {
        
            //calculate the SHA data for input
            status = shaData.update( inBuf, readLen );
        
            if( (0 == status) && (! headerWritten) ) {
                status = writeHeader( _fpTo );
                headerWritten = true;
            }
            
            if( 0 == status ) {
                //if readlen is not a multiple of 8, we have to pad 
                //the input buffer with 0's until the next 8 byte boundary
                remainder = readLen % 8;

                if( 0 != remainder ) {
                    readLen += ( 8 - remainder );
                }

                DES_ede3_cbc_encrypt( 
                        inBuf, 
                        outBuf, 
                        readLen, 
                        &_ks1,
                        &_ks2, 
                        &_ks3, 
                        (DES_cblock *)_iv, 
                        1);

                status = HwUtil::writeToFile( _fpTo, outBuf, readLen );
            }
        }
    }

    //write the sha1 value of input
    if( 0 == status ) {
        status = shaData.final();
        if( 0 == status ) {
            status = HwUtil::writeToFile( _fpTo, shaData.getBuffer(), SHA_DIGEST_LENGTH );
        }
    }
    
    return status;
}


/**
 * Gets the header
 *
 * @param fp                File pointer to write to
 */
int DesEncryptor::writeHeader( FILE * fp ) const {
    
    int status = 0;
    unsigned char remainder = 0;

    //determine how many bytes are in the last block of the file
    remainder = _fileSize % 8;     

    if( 0 == remainder ) {
        remainder = 8;
    }
    
    unsigned char num[1];
    num[0] = remainder;
    
    HwUtil::writeToFile(fp, HwConstant::DES_HEADER);
    HwUtil::writeToFile(fp, num, 1 );

    return status;
}

/**
 * Intitializes the DES keys
 *
 * @param sha1Buf               Sha1 value
 * @param soPath                So Path
 */
int DesEncryptor::initDesKeys( 
        unsigned char sha1Buf[SHA_DIGEST_LENGTH],
        const std::string & soPath ) {

    int status = 0;

    status = DesBase::initDesKeys( sha1Buf, soPath );

    if( 0 == status ) {
        status = validateKey( &_ks1 );
    }
    if( 0 == status ) {
        status = validateKey( &_ks2 );
    }
    if( 0 == status ) {
        status = validateKey( &_ks3 );
    }

    return status;
}


/**
 * Checks to see if the key is valid
 * 
 * @param ks                DES key schedule
 */
int DesEncryptor::validateKey( DES_key_schedule *ks ) const {

     int status = 0;
 
     if( DES_is_weak_key( (DES_cblock *) ks->ks->cblock) ) {
        
        std::string msg = "Key ";
        msg += HwUtil::getHexString(ks->ks->cblock, 8);
        msg += "is a weak key. Try again";

        HwUtil::writeOutput( msg );
    }

    return status;

}
