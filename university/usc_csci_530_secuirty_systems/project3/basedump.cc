#include "basedump.h"
#include "hwconstant.h"
#include "hwutil.h"

#include <openssl/err.h>

/**
 * C'tor.
 *
 * @param filePath          File path
 */
BaseDump::BaseDump( const std::string filePath, const std::string soPath) {
    _filePath = filePath;      
    _soPath = soPath;
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
 * @param mustExist             Parameter to check if the file exists.
 *                              If this parameter is false and the filepath
 *                              is empty, stdin will be the default file pointer.
 */
int BaseDump::openInputFile( bool mustExist ) {

    int status = 0;

    if( _filePath.length() > 0 ) {
        _fp = fopen(_filePath.c_str(), "rb");
    } else {
        if( ! mustExist ) {
            _fp = stdin;    
        }
    }

    if( NULL == _fp ) {
        std::string msg =  "Unable to open input file \"";
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
            _fp = NULL;
        }

        if( 0 != status ) {
            std::string msg =  "Unable to close input file \"";
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

/**
 * Returns the openssl error string 
 *
 * @param errorCode         Error code. 
 *
 * @return                  Error string if available,else "No information available"
 */
std::string BaseDump::getOpenSslErrStr( int errorCode ) const {

    std::string str;
 
    char * error_str = ERR_error_string( ERR_get_error(), NULL );
    if( NULL != error_str ) {
        str += "[ ";
        str += error_str;
        str += " ]";

        str = error_str;
    }

    if( str.length() <= 0 ) {
        str = "No information available"; 
    }

    return str;
}

