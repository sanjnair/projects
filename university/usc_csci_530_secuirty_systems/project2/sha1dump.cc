#include "sha1dump.h"
#include "hwutil.h"
#include "sha1data.h"


/**
 * C'tor.
 *
 * @param filePath          File path
 */
Sha1Dump::Sha1Dump(std::string filePath) : BaseDump(filePath) {
}

/**
 * Virtual D'tor.
 */
Sha1Dump::~Sha1Dump() { }

/**
 * Dumps the output (based on input file or stdin)
 */
int Sha1Dump::dump() {

    int status = 0;
    Sha1Data sha1Data;

    status = openInputFile();

    if( 0 == status ) {
       status = sha1Data.calculate( _fp );
        
        if( 0 == status ) {
            HwUtil::writeOutputLn( sha1Data.getHexString() );
        }

        //close input file
        closeInputFile();
    }
    
    return status;
}
