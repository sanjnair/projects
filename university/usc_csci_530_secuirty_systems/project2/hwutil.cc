#include "hwutil.h"
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>

#include "hwconstant.h"

/**
 * Constructor is private
 */
HwUtil::HwUtil() { }

/**
 * Converts number to string
 */
std::string HwUtil::getString(int number) {

    std::stringstream converter;
    converter << number;
    
    return converter.str();
}

/**
 * Returns Hex string for the given character
 */
std::string HwUtil::getHexString(unsigned char c ) {

    std::string hexChar;
    char buffer[10];
    sprintf(buffer, "%02x", (unsigned char) c); 
   
    hexChar = buffer;

    return hexChar;
}

/**
 * Returns Hex string for the give number
 */
std::string HwUtil::getHexString(unsigned int c ) {

    std::string hexChar;
    char buffer[10];
    sprintf(buffer, "%06x", (unsigned int) c); 
   
    hexChar = buffer;

    return hexChar;
}

/** 
 * Write output in hex format to stdout
 *
 * @param buffer                Buffer to write
 * @param bufferLen             Length of the buffer
 * @param appendNewLine         Insert a newline char at the end
 */
std::string HwUtil::getHexString( 
                const unsigned char * buffer,
                int bufferLen) {

    std::string str;

    if( bufferLen > 0 ) {
        for( int i=0; i<bufferLen; i++ ) {
            str += getHexString( buffer[i] );
        }
    }

    return str;
}

/**
 * Writes the give string to the file pointed b file pointer.
 *
 * @param fp                    Valid file pointer
 * @param str                   String to write
 */
void HwUtil::writeToFile( FILE * fp, const std::string & str) {
	
    fwrite(str.c_str(), sizeof(char), str.length(), fp );
}

/**
 * Writes error to stderr. Program will append a newline after 
 * error being written.
 *
 * @param fp                    Valid file pointer
 * @param str                   String to write
 */
void HwUtil::writeError(const std::string & str) {
    std::string lineStr = str + "\n";
    writeToFile(stderr, lineStr);
}

/**
 * Writes to stdout
 *
 * @param str                   String to write
 */
void HwUtil::writeOutput(const std::string & str) {
    writeToFile(stdout, str);
}

/**
 * Writes to stdout and inserts a new line after the string.
 *
 * @param str                   String to write
 */
void HwUtil::writeOutputLn(const std::string & str) {
    std::string lineStr = str + "\n";
    writeOutput(lineStr);
}

/**
 * Writes to stdout
 *
 * @param buffer                Charracter buffer
 * @param bufferLen             Length of the buffer
 */
void HwUtil::writeOutput( const unsigned char * buffer, unsigned int bufferLen ) {
    fwrite(buffer, sizeof(unsigned char), bufferLen, stdout );
}

/**
 * Sets the last error message
 */
void HwUtil::setLastErrorMessage(const std::string & str ) {
    _errorMsg = str;
}

/**
 * Returns the last error message
 */
std::string HwUtil::getLastErrorMessage() {
    return _errorMsg;
}

/**
 * Returns the file size of the file
 *
 * @param fileName              Name of the file
 * @param size_out              File Size
 *
 * @return                      0, if successful.
 */
int HwUtil::getFileSize( const std::string & fileName,
                         long & size_out ) {
    int status = 0;
    
    size_out = 0;

    #ifdef WINDOWS
        struct _stat buf;
        status = _stat( fileName.c_str(), & buf );
    #else
        struct stat buf;
        status = stat( fileName.c_str(), & buf );
    #endif
    
    if( 0 != status ) {
        HwUtil::setLastErrorMessage("Unable to get the size of the input file");
    } else {
        size_out = buf.st_size;
    }

    return status;
}

/**
 * Creates the buffer and returns the pointer to buffer.
 * If the buffer could not be created, this method returns
 * error code and buffer pointer will points to NULL.
 *
 * NOTE: Caller need to de-allocate the memory after use.
 *
 * @param size                  Size of the buffer
 * @param buffer_ptr            Reference to buffer pointer
 */
int HwUtil::createBuffer( int size, unsigned char *& buffer_ptr ) {
    
    int status = 0;
    buffer_ptr = NULL;
    
    buffer_ptr = new unsigned char[size];
    
    if( NULL == buffer_ptr ) {
        std::string msg = "Unable to allocate memory for the buffer [";
        msg += HwUtil::getString(size);
        msg += "]";

        HwUtil::setLastErrorMessage(msg);
        status = HwConstant::ERROR_OUT_OF_MEMORY;
    } else {
        memset(buffer_ptr, 0, size );
    }

    return status;
}

/**
 * Deletes the buffer
 */
void HwUtil::deleteBuffer( unsigned char *& buffer_ptr ) {
    
    if( NULL != buffer_ptr ) {
        delete [] buffer_ptr;
        buffer_ptr = NULL;
    }
}

/**
 * Creates the buffer and returns the pointer to buffer.
 * If the buffer could not be created, this method returns
 * error code and buffer pointer will points to NULL.
 *
 * NOTE: Caller need to de-allocate the memory after use.
 *
 * @param size                  Size of the buffer
 * @param buffer_ptr            Reference to buffer pointer
 */
int HwUtil::createCharBuffer( int size, char *& buffer_ptr ) {
    
    int status = 0;
    buffer_ptr = NULL;
    
    buffer_ptr = new char[size];
    
    if( NULL == buffer_ptr ) {
        std::string msg = "Unable to allocate memory for the buffer [";
        msg += HwUtil::getString(size);
        msg += "]";

        HwUtil::setLastErrorMessage(msg);
        status = HwConstant::ERROR_OUT_OF_MEMORY;
    } else {
        memset(buffer_ptr, 0, size );
    }

    return status;
}

/**
 * Deletes the buffer
 */
void HwUtil::deleteCharBuffer( char *& buffer_ptr ) {
    
    if( NULL != buffer_ptr ) {
        delete [] buffer_ptr;
        buffer_ptr = NULL;
    }
}

/**
 * Checks the read status of the file. If the read status is not
 * correct, this method will update the error message and returns 
 * error code.
 */
int HwUtil::checkFileReadStatus( FILE * fp ) {
    int status = 0;
    
    if( NULL != fp ) {
        if( ferror(fp) ) {
            std::string msg = "Error: Reading from input resulted in error.";
            HwUtil::setLastErrorMessage(msg);
            status = HwConstant::ERROR_FILE_READ;
        }
    }

    return status;
}


//static declaration
std::string HwUtil::_errorMsg = "";
