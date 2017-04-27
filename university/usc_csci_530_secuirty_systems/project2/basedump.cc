#include "basedump.h"
#include "hwconstant.h"
#include "hwutil.h"

#include <openssl/sha.h>
#include <openssl/md5.h>

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
BaseDump::~BaseDump() { }

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
int BaseDump::checkFileReadStatus() const {
    int status = 0;

    status = HwUtil::checkFileReadStatus( _fp );

    return status;
}
