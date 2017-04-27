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
const int HwConstant::ERROR_INVALID_STATE                   = _errorStart++;
const int HwConstant::ERROR_DES_WEAK_KEY                    = _errorStart++;

const std::string HwConstant::OPTION_ENC_BASE64             = "enc-base64";
const std::string HwConstant::OPTION_DEC_BASE64             = "dec-base64";
const std::string HwConstant::OPTION_ENC_DES                = "enc-des";
const std::string HwConstant::OPTION_DEC_DES                = "dec-des";
const std::string HwConstant::OPTION_MD5                    = "md5";
const std::string HwConstant::OPTION_SHA1                   = "sha1";

const std::string HwConstant::DES_HEADER                    = "DES";

const int HwConstant::BUFFER_SIZE                           = 1024; 
const int HwConstant::MAX_PASSWD_LENGTH                     = 80;
const int HwConstant::DES_KEY_LENGTH                        = 8;
const int HwConstant::DES_HEADER_LENGTH                     = 24;
