/**
 * Starting file for Hw1.
 * This file has the main routine.
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#include "hwconstant.h"
#include "hwutil.h"

#include "hexdump.h"
#include "base64enc.h"
#include "base64dec.h"

/**
 * Returns the usage for this program
 */
std::string getUsage() {
    std::string strUsage;

    strUsage =  "Usage: hw1 " + HwConstant::OPTION_HEX_DUMP   + " [file]\n";
    strUsage += "       hw1 " + HwConstant::OPTION_ENC_BASE64 + " [file]\n";
    strUsage += "       hw1 " + HwConstant::OPTION_DEC_BASE64 + " [file]";
	
    return strUsage;
}

/**
 * Returns true if the option passed is valid
 */
bool isValidOpion(std::string option ) {
  
    bool bStatus = false;

    if( (option == HwConstant::OPTION_HEX_DUMP) ||
        (option == HwConstant::OPTION_ENC_BASE64) ||
        (option == HwConstant::OPTION_DEC_BASE64) ) {
        
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

    if( option == HwConstant::OPTION_HEX_DUMP ) {
        dump_ptr = new HexDump(filePath);
    } else if( option == HwConstant::OPTION_ENC_BASE64 ) {
        dump_ptr = new Base64Enc(filePath);
    } else if( option == HwConstant::OPTION_DEC_BASE64 ) {
        dump_ptr = new Base64Dec(filePath);
    }
    
    return dump_ptr;
}

/**
 * Main routine for hw1
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
            strError  = "hw1: Invalid option\n";
            strError += getUsage();
        }
    } else {
        status = -1;
        strError  = "hw1: Invalid arguments\n";
        strError += getUsage();
    }
    
    if( 0 != status ) {
        HwUtil::writeError(strError);
    }

    return status;
}
