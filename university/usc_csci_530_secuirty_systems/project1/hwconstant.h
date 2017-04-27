/**
 * Constant file for HW1
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
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

    static const std::string OPTION_HEX_DUMP;
    static const std::string OPTION_ENC_BASE64;
    static const std::string OPTION_DEC_BASE64;

    static const int BUFFER_SIZE;
private:
    /**
     * Constructor is private
     */
    HwConstant();

    static int _errorStart;
};

#endif //__HWCONSTANT_H__

