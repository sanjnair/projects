#include "hwconstant.h"
#include "base64dec.h"
#include "hwutil.h"

const int Base64Dec::DEC_CHUNK_MAX_LEN = 4;

/**
 * C'tor.
 *
 * @param filePath          File path
 */
Base64Dec::Base64Dec(std::string filePath) : Base64Dump(filePath) {
    _paddedCount    = 0;        
    _curBitPos      = 0;

    initDecMap();
}

/**
 * Sets the error message with invalid input
 */
void Base64Dec::setInvalidInputError() const {
    std::string msg = "Error: Unable to decode base64 input : Invalid input. (error bit range ";
    msg += HwUtil::getString(_curBitPos+1);
    msg += ":";
    msg += HwUtil::getString(_curBitPos + DEC_CHUNK_MAX_LEN);
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
int Base64Dec::validateEncData( const unsigned char * dataArray )  {

    int status = 0;

    if( _paddedCount > 0 ) {
        status = HwConstant::ERROR_INVALID_INPUT;
    } else {
        unsigned char data = 0;
        for( int i=0; i<DEC_CHUNK_MAX_LEN; i++ ) {
            data = dataArray[i];
            char c = static_cast<char>(data);
            
            if( ! isMapHasKey(c)) { 
                if( '=' == c ) { 
                    _paddedCount++;
                } else {
                    status = HwConstant::ERROR_INVALID_INPUT;
                }
            }

            if( (0 == status) && (_paddedCount > 0) ) {
                if( _paddedCount > 2 ) {
                    status = HwConstant::ERROR_INVALID_INPUT;
                } else {
                    if( i < 2 ) { //padded character is in the first 2 bytes - error    
                        status = HwConstant::ERROR_INVALID_INPUT;
                    } else if( 2 == i ) { //if second last character is padded, last should also be padded
                        c = static_cast<char>(dataArray[3]);
                        if( '=' != c ) { 
                            status = HwConstant::ERROR_INVALID_INPUT;
                        }
                    }
                }
            }
        }
    }

    if( 0 != status ) {
        setInvalidInputError();
    }

    return status;
}

/**
 * Outputs the decoded data for the given 4 bytes of encoded data.
 *
 * @param dataArray             Array of data
 */
int Base64Dec::outputDecodedData( const unsigned char * dataArray ) {
    int status = 0;
    int i = 0;
    char c = '0';
    unsigned char data = 0;
    unsigned int intData = 0;

    for( i=0; i<DEC_CHUNK_MAX_LEN; i++ ) {
        data = dataArray[i];
        c = static_cast<char>(data);

        if( isMapHasKey(c)) { 
            data = _decMap[c];
        } else if( '=' == c ) {
            data = 0;  //padded value
        } else {
            //should never reach here
            status = HwConstant::ERROR_INVALID_INPUT;
            HwUtil::setLastErrorMessage("Invalid data file - error point");
            break;
        }
        
        //we are interested in only the last 6 bits of a byte.
        //So, we'll shift the bits by 2 to the left, so that top
        //6 bits (MSB) are the useful data
        data = (data << 2);

        //shift to the left of the int (first one will go up to 26)
        unsigned int tempIntData = 0;
        tempIntData = data << ((3-i)*6);
        //shift 2 bit out to even the data
        tempIntData = (tempIntData >> 2);

        intData |= tempIntData;
    }
    
    unsigned char outputArray[3] = {0,0,0};
    //get the 3 original bytes
    for(i=0; i<3; i++ ) {
        data = (intData >> (i*8)) & 0xff; //get each byte
        //data is extracted in reverse order, so start stroing from the bottom
        outputArray[2-i] = data;
    }
    
    unsigned char * outputArray_ptr = NULL;
    int arraySize = (3 - _paddedCount); //take care of the padded data towards the end
    status = createBuffer(arraySize, outputArray_ptr );
    
    //check - in case
    assert( (arraySize >0) && (arraySize <= 3) );
    
    if( 0 == status ) {
        for(i=0; i<arraySize;i++){
            outputArray_ptr[i] = outputArray[i];
            
        }

        //print the data to console
        HwUtil::writeOutput( outputArray_ptr, arraySize );
        delete [] outputArray_ptr;
    }

    return status;
}

/**
 * Virtual D'tor.
 */
Base64Dec::~Base64Dec() {}

/**
 * Performs the hex dump operation.
 *
 * @param buffer_ptr        Pointer to buffer
 */
int Base64Dec::performBase64Dec( const unsigned char *buffer_ptr ) {
    
    assert(NULL != buffer_ptr);
    
    int status = 0;

    status = validateEncData(buffer_ptr);

    if( 0 == status ) {
        status = outputDecodedData(buffer_ptr);
        
        _curBitPos += DEC_CHUNK_MAX_LEN; //advance the bit pos by 4
    } 
    
    return status;
}

/**
 * Dumps the output (based on input file or stdin)
 */
int Base64Dec::dump() {

    int status          = 0;
    int readLen         = 0;
    unsigned char data  = 0;
    int curIndex        = 0; //current index within the tempBuffer.
    
    
    unsigned char *buffer_ptr = NULL;
    unsigned char tempBuffer[DEC_CHUNK_MAX_LEN];
    
    status = openInputFile();

    if( 0 == status ) {
        status = createBuffer(HwConstant::BUFFER_SIZE, buffer_ptr );

        if( 0 == status ) {
            while( (!feof(_fp)) && (0 == status) ) {
                readLen = fread(buffer_ptr, 
                                sizeof(unsigned char), 
                                HwConstant::BUFFER_SIZE,
                                _fp );

                status = checkFileReadStatus();

                if( 0 == status ) {
                    if( readLen > 0 ) {
                        for(int i=0; i<readLen; i++) {
                            data = buffer_ptr[i];
                            //ignore the line break characters from the input file
                            if( 10 != data ) {
                                tempBuffer[curIndex] = data;
                                curIndex++;
                                if( DEC_CHUNK_MAX_LEN == curIndex ) {
                                    status = performBase64Dec(tempBuffer); 
                                    curIndex = 0;
                            
                                    if( 0 != status ) {
                                        break; //break for loop in case of error
                                    }
                                }
                            }
                        }
                    }
                }
            }

            //delete the buffer
            delete [] buffer_ptr;
            buffer_ptr = NULL;
        }

        //close input file
        closeInputFile();
    }
    
    if( (0 == status) && (curIndex != 0 ) ) {
        status = HwConstant::ERROR_INVALID_INPUT;
        setInvalidInputError();
    }
    
    if( (0 != status) && (_curBitPos >= DEC_CHUNK_MAX_LEN) ) {
        //insert a blank line if some data got outputted
        //so that error msg can be in a new line
        std::string emptyStr;
        HwUtil::writeOutputLn(emptyStr);
    }

    return status;
}

/**
 * Initializes the decoding map
 */
void Base64Dec::initDecMap() {
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
bool Base64Dec::isMapHasKey( unsigned char key) const {
    bool  status = false;

    Base64DecMap::const_iterator iter = _decMap.find(key);

    if( iter != _decMap.end() ) {
        status = true;
    }
    
    return status;
}
