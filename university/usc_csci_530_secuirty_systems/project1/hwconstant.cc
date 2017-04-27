#include "hwconstant.h"

/**
 * Constructor is private
 */
HwConstant::HwConstant() { }

int HwConstant::_errorStart                                 = 0x40000001;

const int HwConstant::ERROR_FILE_OPEN                       = _errorStart++;
const int HwConstant::ERROR_FILE_CLOSE                      = _errorStart++;
const int HwConstant::ERROR_FILE_READ                       = _errorStart++;
const int HwConstant::ERROR_OUT_OF_MEMORY                   = _errorStart++;
const int HwConstant::ERROR_INVALID_INPUT                   = _errorStart++;

const std::string HwConstant::OPTION_HEX_DUMP               = "hexdump";
const std::string HwConstant::OPTION_ENC_BASE64             = "enc-base64";
const std::string HwConstant::OPTION_DEC_BASE64             = "dec-base64";

const int HwConstant::BUFFER_SIZE                           = 1024; 


