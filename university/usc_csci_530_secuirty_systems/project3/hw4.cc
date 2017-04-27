/**
 * Starting file for Hw4.
 * This file has the main routine.
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/11/2005 - Created
 */
#include "hwconstant.h"
#include "hwutil.h"

#include "basedump.h"
#include "secretdump.h"
#include "signdump.h"
#include "verifydump.h"
#include "desencdump.h"
#include "desdecdump.h"
#include "editordump.h"

/**
 * Returns true, if the sopath option is valid.
 * If valid, true will be returned and the parameter
 * soPath will be populated with the value.
 * Else, false will be returned
 *
 * @param soOption          So option
 * @param soPath            So Path
 */
bool isValidSoPath( 
        const std::string soOption,    
        std::string & soPath ) {

    bool status = false;

    if( soOption.length() > 0 ) {
        
        int option_len = soOption.length();
        int so_len = HwConstant::PARAM_SO.length();

        if( option_len > so_len ) {
            std::string subStr = soOption.substr(0,so_len);

            if( subStr == HwConstant::PARAM_SO ) {
                soPath = soOption.substr( so_len, option_len );
                status = true;
            }
        }
    }

    return status;
}

/** 
 * Returns the option usage
 */
std::string getOptionUsage( 
                const std::string optionName,
                bool isSoNeeded,
                bool isFileParamPresent,
                bool isFileOptional ) {

    std::string str = "       hw4 " + optionName;

    if( isSoNeeded ) {
        str += " " + HwConstant::PARAM_SO + "sopath";
    }

    if( isFileParamPresent ) {
        str += " ";

        if( isFileOptional ) {
            str += "[";
        }
        
        str += "file";

        if( isFileOptional ) {
            str += "]";
        }
    }

    str += "\n";

    return str;
}


/**
 * Returns the dump pointer based on option.
 *
 */
BaseDump * getDumpPtr( 
            const std::string & option, 
            const std::string & filePath,
            const std::string & soPath ) {

    BaseDump * dump_ptr = NULL;

    if( option == HwConstant::OPTION_SECRET ) {
        dump_ptr = new SecretDump( filePath, soPath );
    } else if( option == HwConstant::OPTION_SIGN ) {
        dump_ptr = new SignDump(filePath, soPath );
    } else if( option == HwConstant::OPTION_VERIFY ) {
        dump_ptr = new VerifyDump(filePath, soPath);
    } else if( option == HwConstant::OPTION_ENC ) {
        dump_ptr = new DesEncDump(filePath, soPath);
    } else if( option == HwConstant::OPTION_DEC ) {
        dump_ptr = new DesDecDump(filePath, soPath);
    } else if( option == HwConstant::OPTION_EDIT ) {
        dump_ptr = new EditorDump(filePath, soPath);
    }

    return dump_ptr;
}

/**
 * Returns the usage for this program
 */
std::string getUsage( std::string prefix ) {
    std::string strUsage;

    if( prefix.length() > 0 ) {
        strUsage = prefix + "\n";
    }

    strUsage += getOptionUsage( HwConstant::OPTION_SECRET,  false, false, false );
    strUsage += getOptionUsage( HwConstant::OPTION_SIGN,    true, true, false );
    strUsage += getOptionUsage( HwConstant::OPTION_VERIFY,  true, true, true );
    strUsage += getOptionUsage( HwConstant::OPTION_ENC,     true, true, false );
    strUsage += getOptionUsage( HwConstant::OPTION_DEC,     true, true, false );
    strUsage += getOptionUsage( HwConstant::OPTION_EDIT,    true, true, false );
    
    return strUsage;
}

/**
 * Returns 0 if the input is valid
 *
 * @param argc              main's argc
 * @param argv              main's argv
 * @param option            Commandline Option
 * @param filePath          File Path from command line.
 * @param soPath            Shared object's path from command line.
 * @param strError          Error String
 */
int isValidInput(
        int argc, 
        char * argv[],
        std::string & option, 
        std::string & filePath,
        std::string & soPath,
        std::string & strError ) {
  
    int status = 0;
    
    strError = "";

    if( (argc < 2) || (argc > 4) ) {
        strError  = "Invalid arguments";
    }
    
    if( strError.length() <= 0 ) {
        option = argv[1];

        if( option == HwConstant::OPTION_SECRET ) {
            if(2 != argc) { 
                strError = std::string("Invalid arguments for option ") + option;
            }
        } else if( (option == HwConstant::OPTION_SIGN)  ||
                   (option == HwConstant::OPTION_ENC)   ||
                   (option == HwConstant::OPTION_DEC)   ||
                   (option == HwConstant::OPTION_EDIT) ) {
            
            if( 4 != argc ) {
                strError = std::string("Invalid arguments for option ") + option;
            } else if( ! isValidSoPath(argv[2], soPath) ) {
                strError  = "Invalid SoPath option";
            } else {
                filePath = argv[3];
            }
        } else if( option == HwConstant::OPTION_VERIFY ) {
            if( ! ((argc >= 3) && (argc <= 4)) ) {
                strError = std::string("Invalid arguments for option ") + option;
            } else if( ! isValidSoPath(argv[2], soPath) ) {
                strError  = "Invalid SoPath option";
            } else {
                if( argc > 3 ) {
                    filePath = argv[3];
                }
            }
        } else {
            strError  = std::string("Invalid option ") + option;
        }
    }
    
    if( strError.length() > 0 ) {
        status = HwConstant::ERROR_INVALID_INPUT;
        strError = std::string("hw4 :") + strError;
        strError = getUsage( strError ); 
    }
    
    return status;
}


/**
 * Main routine for Hw4
 */
int main( int argc, char * argv[] ) {
    int status = 0;
    std::string strError;
    std::string strOption;
    std::string strSoPath;
    std::string strFile;
    BaseDump * dump_ptr = NULL;

    status = isValidInput( argc, argv, strOption, strFile, strSoPath, strError );

    if( 0 == status ) {
        dump_ptr = getDumpPtr(strOption, strFile, strSoPath );
        
        if( NULL == dump_ptr ) {
            status = -1;
            strError = "Error: Unable to get the dump ptr";
        }
        if( 0 == status ) {
            status = dump_ptr->dump();
                
            if( 0 != status ) {
                strError  = "Error: ";
                strError  += HwUtil::getLastErrorMessage();
            }
            //delete the allocated memory
            delete dump_ptr;
        }
    }

    if( 0 != status ) {
        HwUtil::writeError(strError);
    }

    return status;
}
