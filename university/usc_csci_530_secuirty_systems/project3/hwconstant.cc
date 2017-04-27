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
const int HwConstant::ERROR_LIBRARY_LOAD                    = _errorStart++;
const int HwConstant::ERROR_LIBRARY_UNLOAD                  = _errorStart++;
const int HwConstant::ERROR_LIBRARY_RESOLVE                 = _errorStart++;

const std::string HwConstant::OPTION_SECRET                 = "secret";
const std::string HwConstant::OPTION_SIGN                   = "sign";
const std::string HwConstant::OPTION_VERIFY                 = "verify";
const std::string HwConstant::OPTION_ENC                    = "enc";
const std::string HwConstant::OPTION_DEC                    = "dec";
const std::string HwConstant::OPTION_EDIT                   = "edit";
const std::string HwConstant::PARAM_SO                      = "-so=";

const std::string HwConstant::ENV_EDITOR                    = "EDITOR";
const std::string HwConstant::ENV_HOME                      = "HOME";
const std::string HwConstant::TMP_DIR                       = "tmp";
const std::string HwConstant::DIR_SEPARATOR                 = "/";

const std::string HwConstant::DES_HEADER                    = "HW4";

const std::string HwConstant::SYMBOL_SC_INIT                = "SC_init";
const std::string HwConstant::SYMBOL_SC_CLEANUP             = "SC_cleanup";
const std::string HwConstant::SYMBOL_SC_GETSIGNATURESIZE    = "SC_get_signature_size";
const std::string HwConstant::SYMBOL_SC_SIGN                = "SC_sign";
const std::string HwConstant::SYMBOL_SC_3DESKEYGEN          = "SC_3des_key_gen";
const std::string HwConstant::SYMBOL_SC_PUBLICSIZEQUERY     = "SC_public_size_query";
const std::string HwConstant::SYMBOL_SC_PUBLICQUERY         = "SC_public_query";

const int HwConstant::BUFFER_SIZE                           = 1024; 
const int HwConstant::MAX_PASSWD_LENGTH                     = 80;
const int HwConstant::DES_HEADER_LENGTH                     = 4;

const std::string HwConstant::RSA_PRIVATE_KEY               = "rsa_private_key";
const std::string HwConstant::RSA_PUBLIC_EXPONENT           = "rsa_public_exponent";
const std::string HwConstant::RSA_PUBLIC_MODULUS            = "rsa_public_modulus";
const std::string HwConstant::RSA_RANDOM_BITS_1             = "random_bits_1";
const std::string HwConstant::RSA_RANDOM_BITS_2             = "random_bits_2";

const int HwConstant::RSA_PUBLIC_MODULUS_SIZE               = 1024;
