#include "md5dump.h"
#include "hwutil.h"
#include "md5data.h"

/**
 * C'tor.
 *
 * @param filePath          File path
 */
Md5Dump::Md5Dump(std::string filePath) : BaseDump(filePath) {
    
}

/**
 * Virtual D'tor.
 */
Md5Dump::~Md5Dump() {}

/**
 * Dumps the output (based on input file or stdin)
 */
int Md5Dump::dump() {

    int status = 0;
    Md5Data md5Data;

    status = openInputFile();

    if( 0 == status ) {
       status = md5Data.calculate( _fp );
        
        if( 0 == status ) {
            HwUtil::writeOutputLn( md5Data.getHexString() );
        }

        //close input file
        closeInputFile();
    }
    
    return status;
 
}
