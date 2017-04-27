#include "hwconstant.h"
#include "hwutil.h"
#include "hexdump.h"

const int HexDump::HEX_CHUNK_MAX_LEN = 16;

/**
 * C'tor.
 *
 * @param filePath          File path
 */
HexDump::HexDump(std::string filePath) : BaseDump(filePath) {
    _hexLineValue = 0;
}

/**
 * Virtual D'tor.
 */
HexDump::~HexDump() {}

/**
 * Performs the hex dump operation.
 *
 * @param buffer_ptr        Pointer to buffer
 * @param bufferLen         Length of the buffer
 */
int HexDump::performHexDump( unsigned char *buffer_ptr, int bufferLen ) {
    int status = 0;
    unsigned char data = 0;
    unsigned int count = 0;
    
    std::string strLine;
    std::string strLineCode;
    std::string strLineAscii;

    assert( (bufferLen>0) && bufferLen <= HEX_CHUNK_MAX_LEN);

    for(int i=0; i<bufferLen; i++ ) {
        
        if( 0 == count ) {
            strLine  = HwUtil::getHexString(_hexLineValue);
            strLine += ": ";
        }
        
        data = buffer_ptr[i];
        strLineCode += HwUtil::getHexString(data);
        strLineCode += " ";

        //ascii data
        if( (data >= 0x20) && (data <= 0x7e) ) {
            strLineAscii += (char) data;;
        } else if( (data <= 0x1f) || (data == 0x7f) ) {
            strLineAscii += ".";
        } else {
            strLineAscii += "~";
        }

        if( 7 == count ) {
            strLineCode += " ";
        } 
        
        if( (i == (bufferLen-1)) || (15 == count) ) {
            
            if( 15 != count ) {
                unsigned int remaining = 15 - count;
                if( remaining > 0 ) {
                    for( unsigned int i=0; i< remaining; i++ ) {
                        strLineCode += "-- ";
                        strLineAscii += " ";
                        count++;

                        if( 7 == count ) {
                            strLineCode += " ";
                        }
                    }
                }
            }
            
            strLine += strLineCode;
            strLine += " ";
            strLine += strLineAscii;

            _hexLineValue += 0x10;
            strLineCode = "";
            strLineAscii = "";
            count = 0;

            HwUtil::writeOutputLn(strLine);

        } else {
            count++;
        }
    }
   
    return status;
}

/**
 * Dumps the output (based on input file or stdin)
 */
int HexDump::dump() {

    int status      = 0;
    int readLen     = 0;
    int curIndex    = 0; //current index within the tempBuffer.
    
    unsigned char *buffer_ptr = NULL;
    unsigned char tempBuffer[HEX_CHUNK_MAX_LEN];

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
                        //fill the temporary buffer
                        for(int i=0; i<readLen; i++) {
                            tempBuffer[curIndex] = buffer_ptr[i];
                            curIndex++;
                    
                            if( HEX_CHUNK_MAX_LEN == curIndex ) {
                                status = performHexDump(tempBuffer,HEX_CHUNK_MAX_LEN); 
                                curIndex = 0;
                    
                                if( 0 != status ) {
                                    break; //break for loop in case of error
                                }
                            }
                        }
                    }
                }

            }
            //delete the buffer
            delete [] buffer_ptr;
            buffer_ptr = NULL;

            //look for any items in the buffer. if yes, perform hexdump
            if( (0 == status) && (curIndex > 0) ) {
                status = performHexDump(tempBuffer, curIndex ); 
                curIndex = 0;
            }

        } 
        //close input file
        closeInputFile();
    }
    
    return status;
}

