/**
 * Starting file for Hw2.
 * This file has the main routine.
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#include "hwconstant.h"
#include "hwutil.h"

#include "base64encdump.h"
#include "base64decdump.h"
#include "sha1dump.h"
#include "md5dump.h"
#include "desencdump.h"
#include "desdecdump.h"


/**
 * Returns the usage for this program
 */
std::string getUsage() {
    std::string strUsage;

    strUsage += "       hw2 " + HwConstant::OPTION_ENC_BASE64   + " [file]\n";
    strUsage += "       hw2 " + HwConstant::OPTION_DEC_BASE64   + " [file]\n";
    strUsage += "       hw2 " + HwConstant::OPTION_MD5          + " [file]\n";
    strUsage += "       hw2 " + HwConstant::OPTION_SHA1         + " [file]\n";
    strUsage += "       hw2 " + HwConstant::OPTION_ENC_DES      + " file\n";
    strUsage += "       hw2 " + HwConstant::OPTION_DEC_DES      + " file\n";
	
    return strUsage;
}

/**
 * Returns true if the option passed is valid
 */
bool isValidOpion(std::string option ) {
  
    bool bStatus = false;

    if( (option == HwConstant::OPTION_ENC_BASE64) ||
        (option == HwConstant::OPTION_DEC_BASE64) ||
        (option == HwConstant::OPTION_MD5)        ||
        (option == HwConstant::OPTION_SHA1)       ||
        (option == HwConstant::OPTION_ENC_DES)    ||
        (option == HwConstant::OPTION_DEC_DES) ) {
        
        bStatus = true;
    }
    
    return bStatus;
}

/**
 * Returns the dump pointer based on option.
 *
 */
BaseDump * getDumpPtr( std::string option, std::string filePath ) {
    BaseDump * dump_ptr = NULL;

    if( option == HwConstant::OPTION_ENC_BASE64 ) {
        dump_ptr = new Base64EncDump(filePath);
    } else if( option == HwConstant::OPTION_DEC_BASE64 ) {
        dump_ptr = new Base64DecDump(filePath);
    } else if( option == HwConstant::OPTION_MD5 ) {
        dump_ptr = new Md5Dump(filePath);
    } else if( option == HwConstant::OPTION_SHA1 ) {
        dump_ptr = new Sha1Dump(filePath);
    } else if( option == HwConstant::OPTION_ENC_DES ) {
        dump_ptr = new DesEncDump(filePath);
    } else if( option == HwConstant::OPTION_DEC_DES ) {
        dump_ptr = new DesDecDump(filePath);
    }

    return dump_ptr;
}

/**
 * Main routine for Hw2
 */
int main( int argc, char * argv[] ) {
    int status = 0;
    std::string strError;
    std::string strOption;
    std::string strFile;
    BaseDump * dump_ptr = NULL;

    if( (2 == argc) || (3 == argc) ) {
        strOption = argv[1];
        
        if( isValidOpion(strOption) ) {
            if( 3 == argc ) {
                strFile = argv[2];
            }
            
            dump_ptr = getDumpPtr(strOption, strFile );

            if( NULL != dump_ptr ) {
                status = dump_ptr->dump();
                
                if( 0 != status ) {
                    strError  = HwUtil::getLastErrorMessage();
                }
                //delete the allocated memory
                delete dump_ptr;
            } else {
                strError = "Unable to get the dump ptr";
            }
        } else {
            status = -1;
            strError  = "Hw2: Invalid option\n";
            strError += getUsage();
        }
    } else {
        status = -1;
        strError  = "Hw2: Invalid arguments\n";
        strError += getUsage();
    }
    
    if( 0 != status ) {
        HwUtil::writeError(strError);
    }

    return status;
}
