#include "hwconstant.h"
#include "base64enc.h"
#include "hwutil.h"

const int Base64Enc::ENC_CHUNK_MAX_LEN = 3;

/**
 * C'tor.
 *
 * @param filePath          File path
 */
Base64Enc::Base64Enc(std::string filePath) : Base64Dump(filePath) {
    initEncMap();
}

/**
 * Virtual D'tor.
 */
Base64Enc::~Base64Enc() {}

/**
 * Returns the 4 ASCII characters for the 3 bytes 
 *
 * @param valueArray        Array where the bytes are stored
 * @param padLen            Length of the padded items
 */
std::string Base64Enc::getEncodedString( 
                            const unsigned char * buffer_ptr, 
                            int padLen ) const {
    
    int i                   = 0;
    int loopSize            = 0;
    unsigned int intVal     = 0;
    std::string strEncoded  = "";
    char encodedChar        = '@';
    
    assert(padLen <= 2);

    unsigned char valueArray[ENC_CHUNK_MAX_LEN];
    memset(valueArray,0,ENC_CHUNK_MAX_LEN);

    loopSize = ENC_CHUNK_MAX_LEN - padLen;
    
    //only set relevant values
    for(i=0; i< loopSize; i++) {
        valueArray[i] = buffer_ptr[i];
    }
    
    intVal = (valueArray[0] << 16) + (valueArray[1] << 8) + valueArray[2];

    //shift out 6 bits each and store it in the array
    for(i=0; i<4; i++) {
        unsigned int mask = 0x3f << (i*6); //mask out 6 bits each
        unsigned char value = static_cast<unsigned char>( ( (intVal & mask) >>(i*6) ) );

        if( i < padLen ) {
            encodedChar = '=';
        } else {
            encodedChar = getBase64Char(value);
        }
        
        std::string::iterator it = strEncoded.begin();
        strEncoded.insert(it, encodedChar );
    }

    return strEncoded;
}


/**
 * Performs the hex dump operation.
 *
 * @param buffer_ptr        Pointer to buffer
 * @param bufferLen         Length of the buffer
 * @param strLine           String line (reference);
 */
int Base64Enc::performBase64Enc( 
                    const unsigned char *buffer_ptr, 
                    int bufferLen,
                    std::string & strLine ) {
    
    assert( (bufferLen>0) && (bufferLen <= ENC_CHUNK_MAX_LEN) );

    int status      = 0;
    int padSize     = 0;
    
    padSize = ENC_CHUNK_MAX_LEN - bufferLen;
    
    //get the encoded string
    strLine += getEncodedString(buffer_ptr, padSize);

    //insert a line feed after every 64 characters.
    if( strLine.length() == 64 ) {
        HwUtil::writeOutputLn(strLine);
        strLine = "";
    }
    
    return status;
}

/**
 * Dumps the output (based on input file or stdin)
 */
int Base64Enc::dump() {

    int status          = 0;
    int readLen         = 0;
    int curIndex        = 0; //current index within the tempBuffer.
    unsigned char data  = 0;

    std::string strLine = "";

    unsigned char *buffer_ptr = NULL;
    unsigned char tempBuffer[ENC_CHUNK_MAX_LEN];
    //clear the array
    memset(tempBuffer,0,ENC_CHUNK_MAX_LEN);
    
    status = openInputFile();

    if( 0 == status ) {
        status = createBuffer(HwConstant::BUFFER_SIZE, buffer_ptr );

        if( 0 == status ) {
            while( (!feof(_fp)) && (0 == status) ) {
                memset(buffer_ptr, 0, HwConstant::BUFFER_SIZE );
                readLen = fread( buffer_ptr, 
                                 sizeof(unsigned char), 
                                 HwConstant::BUFFER_SIZE,
                                 _fp );

                status = checkFileReadStatus();

                if( 0 == status ) {
                    if( readLen > 0 ) {

                        for(int i=0; i<readLen; i++) {
                            data = buffer_ptr[i];
                            tempBuffer[curIndex] = data;
                            curIndex++;
                            
                            if( ENC_CHUNK_MAX_LEN == curIndex ) {
                                status = performBase64Enc(tempBuffer,curIndex, strLine); 
                                curIndex = 0;
                        
                                if( 0 != status ) {
                                    break; //break for loop in case of error
                                }
                            }
                        }
                    }
                }
            }
            
            if( (0 == status) && (curIndex > 0 ) ) {
                status = performBase64Enc(tempBuffer,curIndex, strLine); 
                curIndex = 0;
            }
            
            //if any string left, write it to output and insert a linefeed
            if( 0 == status ) {
                if( strLine.length() > 0 ) {
                    HwUtil::writeOutputLn(strLine);
                }
            }

            //delete the buffer
            delete [] buffer_ptr;
            buffer_ptr = NULL;
        }

        //close input file
        closeInputFile();
    }
    
    return status;
}

/**
 * Initializes the encoding map
 */
void Base64Enc::initEncMap() {
    unsigned char index = 0;
    unsigned char i = 0;
    
    //fill A-Z
    for(i=0;i<26; i++ ) {
        char c = char(i + 65); //ascii 65 = A
        _encMap[index++] = c;    
    }
    //fill a-z
    for(i=0;i<26; i++ ) {
        char c = char(i + 97); //ascii 97 = a
        _encMap[index++] = c;    
    }
    //fill 0-9
    for(i=0;i<10; i++ ) {
        char c = char(i + 48); //ascii 48 = 0
        _encMap[index++] = c;    
    }
    _encMap[index++] = '+';
    _encMap[index++] = '/';
}

/**
 * Returns the Base 64 encoding character for given value
 */
char Base64Enc::getBase64Char(unsigned char value) const {

    char c = '0';
    Base64EncMap::const_iterator iter = _encMap.find(value);

    if( iter != _encMap.end() ) {
        c = iter->second;
    }

    return c;
}
