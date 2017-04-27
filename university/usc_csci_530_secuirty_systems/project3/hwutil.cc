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
    sprintf(buffer, "%02x", c); 
   
    hexChar = buffer;

    return hexChar;
}

/**
 * Returns Hex string for the give number
 */
std::string HwUtil::getHexString(unsigned int c ) {

    std::string hexChar;
    char buffer[10];
    sprintf(buffer, "%06x",  c); 
   
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
 * Opens the file. 
 * 
 * @param filePath              File Path
 * @param mode                  Mode
 * @param fp                    File pointer
 */
int HwUtil::openFile( 
                const std::string & filePath,
                const std::string & mode,
                FILE *& fp ) {

    int status = 0;

    if( filePath.length() > 0 ) {
        fp = fopen(filePath.c_str(), mode.c_str() );
    } 
    
    if( NULL == fp ) {
        std::string msg =  "Unable to open file \"";
        msg += filePath;
        msg += "\" for reading.";
        HwUtil::setLastErrorMessage(msg);

        status = HwConstant::ERROR_FILE_OPEN;; 
    }
       
    return status;

}

/**
 * Closes the file
 *
 * @param fp                    File pointer
 */
int HwUtil::closeFile( FILE * fp ) {

    int status = 0;

    if( NULL != fp ) {
        status = fclose( fp);

        if( 0 != status ) {
            std::string msg =  "Unable to close file";
            HwUtil::setLastErrorMessage(msg);

            status = HwConstant::ERROR_FILE_CLOSE;
        }
    }

    return status;
}

/**
 * Writes the give string to the file pointed b file pointer.
 *
 * @param data                  Data
 * @param size                  Size
 * @param dataSize              Size of data
 * @param fp                    File pointer
 */
int HwUtil::writeToFile( 
                const void * data,
                unsigned int size,
                unsigned int dataSize,
                FILE * fp ) {
    
    int status = 0;
    std::string msg;
    
    if( NULL != fp ) {
        errno = 0;
        fwrite( data, dataSize, size, fp );
        
        if( 0 != errno  ) {
            msg = "HwUtil::writeToFile: Failed to write to file.";
        }

    } else {
        msg = "HwUtil::writeToFile: File handle is invalid";
    }

    if( msg.size() > 0 ) {
        status = HwConstant::ERROR_INVALID_INPUT;                
        HwUtil::setLastErrorMessage(msg);
    }

    return status;
}

/**
 * Writes the give string to the file pointed b file pointer.
 *
 * @param fp                    Valid file pointer
 * @param str                   String to write
 */
int HwUtil::writeToFile( 
                FILE * fp, 
                const std::string & str) {

    return ( writeToFile( str.c_str(), str.size(), sizeof(char), fp) );
}

/**
 * Writes to file
 *
 * @param fp                    File pointer
 * @param buffer                Charracter buffer
 * @param bufferLen             Length of the buffer
 */
int HwUtil::writeToFile( 
                FILE * fp,
                const unsigned char * buffer, 
                unsigned int bufferLen ) {

    return ( writeToFile( buffer, bufferLen, sizeof(unsigned char), fp) );
}

/**
 * Writes to stdout
 *
 * @param buffer                Charracter buffer
 * @param bufferLen             Length of the buffer
 */
int HwUtil::writeOutput( 
                const unsigned char * buffer, 
                unsigned int bufferLen ) {

    return ( writeToFile( stdout, buffer, bufferLen ) );
}



/**
 * Writes error to stderr. Program will append a newline after 
 * error being written.
 *
 * @param fp                    Valid file pointer
 * @param str                   String to write
 * @param newLine               Append newline
 */
int HwUtil::writeError(const std::string & str, bool newLine ) {
    
    std::string lineStr = str;

    if( newLine ) {
        lineStr += "\n";
    }

    return ( writeToFile(stderr, lineStr) );
}

/**
 * Writes to stdout
 *
 * @param str                   String to write
 */
int HwUtil::writeOutput(const std::string & str) {
    
    return ( writeToFile(stdout, str) );
}

/**
 * Writes to stdout and inserts a new line after the string.
 *
 * @param str                   String to write
 */
int HwUtil::writeOutputLn(const std::string & str) {
    std::string lineStr = str + "\n";
    
    return ( writeOutput(lineStr) );
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
            std::string msg = "Reading from input resulted in error.";
            HwUtil::setLastErrorMessage(msg);
            status = HwConstant::ERROR_FILE_READ;
        }
    }

    return status;
}

/**
 * Gets the value of the environment variable.
 * If value is null, then this method will return a non-zero return value.
 *
 * @param strName               Name of the environment variable
 * @param strValue              Valud of the environment variable
 */
int HwUtil::getEnvValue( 
                const std::string & strName,
                std::string & strValue ) {

    int status = 0;
    
    char * value = getenv( strName.c_str() );
    if( NULL == value ) {
        std::string strMsg = "Unable to get the environment variable ";
        strMsg += strName;
        HwUtil::setLastErrorMessage( strMsg );

        status = HwConstant::ERROR_INVALID_INPUT;
    } else {
        strValue = value;
    }

    return status;
}


//static declaration
std::string HwUtil::_errorMsg = "";
