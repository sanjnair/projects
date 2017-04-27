/**
 * Constant file for HW1
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   02/07/2006 - Created
 */

#ifndef __HWCONSTANT_H__
#define __HWCONSTANT_H__

#include <string>

class HwConstant {

public:
    static const int ERROR_FILE_OPEN;
    static const int ERROR_FILE_CLOSE;
    static const int ERROR_FILE_READ;
    static const int ERROR_OUT_OF_MEMORY;
    static const int ERROR_INVALID_INPUT;
    static const int ERROR_INVALID_STATE;
    static const int ERROR_DES_WEAK_KEY;

    static const std::string OPTION_ENC_BASE64;
    static const std::string OPTION_DEC_BASE64;
    static const std::string OPTION_ENC_DES;
    static const std::string OPTION_DEC_DES;
    static const std::string OPTION_MD5;
    static const std::string OPTION_SHA1;

    static const std::string DES_HEADER;

    
    static const int BUFFER_SIZE;
    static const int MAX_PASSWD_LENGTH;
    static const int DES_KEY_LENGTH;
    static const int DES_HEADER_LENGTH;

private:
    /**
     * Constructor is private
     */
    HwConstant();

    static int _errorStart;
};

#endif //__HWCONSTANT_H__

