#include "base64decdump.h"
#include "hwconstant.h"
#include "hwutil.h"
#include <openssl/evp.h>

//-----------------------------------------------------------
//declare the callback funciton for file BIO
long BIO_debug_callback_file( 
            BIO *bio, 
            int cmd, 
            const char *argp, 
            int argi, 
            long argl, 
            long ret );

Base64DecDump * Base64DecDump::_instance    = NULL;
const int Base64DecDump::DEC_CHUNK_MAX_LEN  = 4;
//-----------------------------------------------------------

/**
 * C'tor.
 *
 * @param filePath          File path
 */
Base64DecDump::Base64DecDump(std::string filePath) : BaseDump(filePath) {

    _bError         = false;
    _paddedCount    = 0;
    _curBitPos      = 0;
    _curIndex       = 0;
    _firstPadIndex  = 0;
    _decBuffer      = NULL;

    initDecMap();
}

/**
 * Virtual D'tor.
 */
Base64DecDump::~Base64DecDump() {}

/**
 * Return the instance of base64
 */
Base64DecDump * Base64DecDump::getInstance() {
    return _instance;
}


/**
 * Dumps the output (based on input file or stdin)
 */
int Base64DecDump::dump() {
    
    int status                  = 0;
    int fileReadLen             = 0;
    unsigned char *inBuf_ptr    = NULL;
    BIO *bio_ptr                = NULL;
    BIO *bioOut_ptr             = NULL;

    status = openInputFile();

    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, inBuf_ptr );

        if( 0 == status ) {
            status = HwUtil::createCharBuffer(DEC_CHUNK_MAX_LEN, _decBuffer );
        }
        
        
        if( 0 == status ) {
            status = createOutBIO( bioOut_ptr );
        }
        
        if( 0 == status ) {
            status = createB64BIO( bio_ptr, inBuf_ptr, fileReadLen );
            
            if( 0 == status ) {
                _instance = this;

                while( (0 == status ) && 
                       (fileReadLen = BIO_read( bio_ptr, inBuf_ptr, HwConstant::BUFFER_SIZE) ) > 0) {

                    if( _bError ) {
                        status = HwConstant::ERROR_INVALID_INPUT;
                        setInvalidInputError();
                        break;
                    } else {
                        BIO_write( bioOut_ptr, inBuf_ptr, fileReadLen );
                    }
                }
                
                if( _bError ) {
                    status = HwConstant::ERROR_INVALID_INPUT;
                    setInvalidInputError();
                }

                _instance = NULL;
            }

            deleteBIO( bio_ptr );
        }

        if( 0 == status ) {
            //no more data in the buffer. Input file is a valid base64 file
            if( 0 == _curIndex ) {
                BIO_flush( bioOut_ptr );
            } else {
                status = HwConstant::ERROR_INVALID_INPUT;
                setInvalidInputError();
            }
        }
            
        deleteBIO( bioOut_ptr );
        HwUtil::deleteCharBuffer( _decBuffer );
        HwUtil::deleteBuffer( inBuf_ptr );
            
        //close input file
        closeInputFile();
    }
       
    return status;
}

/**
 * Sets the error message with invalid input
 */
void Base64DecDump::setInvalidInputError() const {
    std::string msg = "Error: Unable to decode base64 input : Invalid input. (invalid byte# = ";
    msg += HwUtil::getString( _curBitPos+1 );
    msg += ")";

    HwUtil::setLastErrorMessage(msg);
}

/**
 * Validates the Encoded data to see if it follows 
 * base-64 encoding standard. If = sign is present,
 * it has to be the last two.
 *
 * @param dataArray         Array of data
 * 
 */
int Base64DecDump::validateEncData( const char * dataArray )  {

    int status = 0;
    char data = 0;

    for( int i=0; i<DEC_CHUNK_MAX_LEN; i++ ) {
        data = dataArray[i];
        
        if( ! isMapHasKey( data )) { 
            if( '=' == data ) { 
                _paddedCount++;
            } else {
                status = HwConstant::ERROR_INVALID_INPUT;
            }
        }

        if( (0 == status) && (_paddedCount > 0) ) {
            if( 1 == _paddedCount ) {
                _firstPadIndex = _curBitPos;
            } else if( 2 == _paddedCount ) {
                //last character should have been the first padded character
                if( _firstPadIndex != (_curBitPos - 1) ) {
                    status = HwConstant::ERROR_INVALID_INPUT;
                }
            } else {
                status = HwConstant::ERROR_INVALID_INPUT;
            }
        }

        //increment the current bit position if there are no errors
        if( 0 == status ) {
            _curBitPos++;
        } else {
            setInvalidInputError();
            break;
        }
    }

    return status;
}


/**
 * Verifies the input data. If the data is not valid, it will set the 
 * _bError flag to false
 *
 * @param buffer_ptr        Input Buffer
 * @param bufferLen         Length of the buffer
 */
void Base64DecDump::verifyInputData(
        const char * buffer_ptr,
        unsigned int bufferLen ) {

    int status = 0;
    char data = '0';

    if( (! _bError)  && (bufferLen > 0) ) {
        
        for(unsigned int i=0; i<bufferLen; i++) {
            data = buffer_ptr[i];
            //ignore the line break characters from the input file
            if( 10 != data ) {
                _decBuffer[_curIndex] = data;
                _curIndex++;
                if( DEC_CHUNK_MAX_LEN == _curIndex ) {
                    status = validateEncData( _decBuffer ); 
                    _curIndex = 0;
        
                    if( 0 != status ) {
                        _bError = true;
                        break; //break for loop in case of error
                    }
                }
            }
        }
    }
}

/**
 * Creates Base64 BIO
 *
 * @param bio               BIO to be created
 * @param buffer            Memory buffer
 * @param bufferLen         Length of the buffer
 *
 * @return      returns 0 if creation is successful
 */
int Base64DecDump::createB64BIO( 
        BIO *& bio, 
        unsigned char * buffer,  
        unsigned int bufferLen ) const {

    int status = HwConstant::ERROR_OUT_OF_MEMORY;
    
    //bio = BIO_new_mem_buf( buffer, bufferLen );
    bio = BIO_new_fp( _fp, BIO_NOCLOSE );
    
    //set up the bio's for transformation [bio]-->[base64]-->[memory]
    if( NULL != bio ) {
        
        BIO * bio64_ptr = BIO_new( BIO_f_base64() );

        if( NULL != bio64_ptr ) {
            //set callback
            BIO_set_callback( bio, &BIO_debug_callback_file );
            bio = BIO_push( bio64_ptr, bio );
            status = 0;
        }
    }

    if( 0 != status) {
        HwUtil::setLastErrorMessage("Unable to create Memory BIO");
        deleteBIO( bio );
    }

    return status;
}

            
/**
 * Creates output BIO
 *
 * @param bio               BIO to be created
 * @return      returns 0 if creation is successful
 */
int Base64DecDump::createOutBIO( BIO *& bio ) const {

    int status = HwConstant::ERROR_OUT_OF_MEMORY;

    bio = BIO_new_fp( stdout, BIO_NOCLOSE );

    if( NULL != bio ) {
        BIO_set_write_buf_size( bio, HwConstant::BUFFER_SIZE );
        status = 0;
    } else {
        HwUtil::setLastErrorMessage("Unable to create Output BIO");
    }

    return status;
}

/**
 * Deletes specied BIO
 *
 * @param bio               BIO to be deleted
 *
 * @return      returns 0 if deletion is successful
 */
void Base64DecDump::deleteBIO( BIO *& bio ) const {
    
    if( NULL != bio ) {
        BIO_free_all( bio );
        bio = NULL;
    }
}

 /** 
  * Initializes the decoding map
  */
void Base64DecDump::initDecMap() {
    unsigned char index = 0;
    unsigned char i = 0;
    
    //fill A-Z
    for(i=0;i<26; i++ ) {
        char c = char(i + 65); //ascii 65 = A
        _decMap[c] = index++;    
    }
    //fill a-z
    for(i=0;i<26; i++ ) {
        char c = char(i + 97); //ascii 97 = a
        _decMap[c] = index++;    
    }
    //fill 0-9
    for(i=0;i<10; i++ ) {
        char c = char(i + 48); //ascii 48 = 0
        _decMap[c] = index++;    
    }
    _decMap['+'] = index++;
    _decMap['/'] = index++;
}

/**
 * Returns true, if the map contains the given key
 */
bool Base64DecDump::isMapHasKey( unsigned char key) const {
    bool  status = false;

    Base64DecDumpMap::const_iterator iter = _decMap.find(key);

    if( iter != _decMap.end() ) {
        status = true;
    }
    
    return status;
}

/**
 * Callback method 
 *
 */
long BIO_debug_callback_file( 
        BIO *bio, int cmd, const char *argp,
        int argi, long argl, long ret)	{

    long retValue = 1;

    if (BIO_CB_RETURN & cmd) {
		retValue = ret;
    }

    switch (cmd) {
        case BIO_CB_RETURN|BIO_CB_READ: {
            Base64DecDump * b64_ptr = Base64DecDump::getInstance();
            unsigned int readLen = 0;
            
            if( (NULL != bio) && ( NULL != b64_ptr ) ) {
                readLen = static_cast<unsigned int>( ret );

                if( readLen > 0 ) {
                    b64_ptr->verifyInputData( 
                                argp, 
                                readLen );
                }
            }
		    break;
        }
    }
    
    return( retValue);
}

