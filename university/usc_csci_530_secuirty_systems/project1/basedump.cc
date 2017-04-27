#include "basedump.h"
#include "hwconstant.h"
#include "hwutil.h"


/**
 * C'tor.
 *
 * @param filePath          File path
 */
BaseDump::BaseDump(std::string filePath) {
    _filePath = filePath;      
    _fp = NULL;
}

/**
 * Virtual D'tor.
 */
BaseDump::~BaseDump() {}

/**
 * Opens the input file. If the file path is valid and open is successful,
 * this methid creates a valid file pointer and keeps it. In this case, the
 * return code will be 0. Otherwise, return code will be non-zero.
 * 
 */
int BaseDump::openInputFile() {

    int status = 0;

    if( _filePath.length() > 0 ) {
        _fp = fopen(_filePath.c_str(), "rb");
    } else{
        _fp = stdin;
    }

    if( NULL == _fp ) {
        std::string msg =  "Error: Unable to open input file \"";
        msg += _filePath;
        msg += "\" for reading.";
        HwUtil::setLastErrorMessage(msg);

        status = HwConstant::ERROR_FILE_OPEN;; 
    }
       
    return status;
}

/**
 * Opens the input file. If the file path is valid and open is successful,
 * this methid creates a valid file pointer and keeps it. In this case, the
 * return code will be 0. Otherwise, return code will be non-zero.
 * 
 */
int BaseDump::closeInputFile() {

    int status = 0;

    if( NULL != _fp ) {
        if( _filePath.length() > 0 ) {
            status = fclose(_fp);
        }

        if( 0 != status ) {
            std::string msg =  "Error: Unable to close input file \"";
            msg += _filePath;
            msg += "\"";
            HwUtil::setLastErrorMessage(msg);

            status = HwConstant::ERROR_FILE_CLOSE;
        }
    }

    return status;
}

/**
 * Checks the read status of the file. If the read status is not
 * correct, this method will update the error message and returns 
 * error code.
 */
int BaseDump::checkFileReadStatus() {
    int status = 0;
    
    if( NULL != _fp ) {
        if( ferror(_fp) ) {
            std::string msg = "Error: Reading from input resulted in error.";
            HwUtil::setLastErrorMessage(msg);
            status = HwConstant::ERROR_FILE_READ;
        }
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
int BaseDump::createBuffer( int size, unsigned char *& buffer_ptr ) const {
    
    int status = 0;
    buffer_ptr = NULL;
    
    buffer_ptr = new unsigned char[size];
    
    if( NULL == buffer_ptr ) {
        std::string msg = "Unable to allocate memory for the buffer [";
        msg += HwUtil::getString(size);
        msg += "]";

        HwUtil::setLastErrorMessage(msg);
        status = HwConstant::ERROR_OUT_OF_MEMORY;
    }

    return status;
}
