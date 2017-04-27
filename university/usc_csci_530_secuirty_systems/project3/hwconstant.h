/**
 * Constant file for HW1
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
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
    static const int ERROR_LIBRARY_LOAD;
    static const int ERROR_LIBRARY_UNLOAD;
    static const int ERROR_LIBRARY_RESOLVE;

    static const std::string OPTION_SECRET;
    static const std::string OPTION_SIGN;
    static const std::string OPTION_VERIFY;
    static const std::string OPTION_ENC;
    static const std::string OPTION_DEC;
    static const std::string OPTION_EDIT;
    static const std::string PARAM_SO;

    static const std::string ENV_EDITOR;
    static const std::string ENV_HOME;
    static const std::string TMP_DIR;
    static const std::string DIR_SEPARATOR;

    static const std::string DES_HEADER;

    static const std::string SYMBOL_SC_INIT;
    static const std::string SYMBOL_SC_CLEANUP;
    static const std::string SYMBOL_SC_GETSIGNATURESIZE;
    static const std::string SYMBOL_SC_SIGN;
    static const std::string SYMBOL_SC_3DESKEYGEN;
    static const std::string SYMBOL_SC_PUBLICSIZEQUERY;
    static const std::string SYMBOL_SC_PUBLICQUERY;
   
    static const int BUFFER_SIZE;
    static const int MAX_PASSWD_LENGTH;
    static const int DES_HEADER_LENGTH;

    static const std::string RSA_PRIVATE_KEY;
    static const std::string RSA_PUBLIC_EXPONENT;
    static const std::string RSA_PUBLIC_MODULUS;
    static const std::string RSA_RANDOM_BITS_1;
    static const std::string RSA_RANDOM_BITS_2;

    static const int RSA_PUBLIC_MODULUS_SIZE;

private:
    /**
     * Constructor is private
     */
    HwConstant();

    static int _errorStart;
};

#endif //__HWCONSTANT_H__

