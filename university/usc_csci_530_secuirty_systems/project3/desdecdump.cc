#include "desdecdump.h"
#include "desdecryptor.h"

/**
 * C'tor.
 *
 * @param filePath          File path
 */
DesDecDump::DesDecDump(
                const std::string filePath,
                const std::string soPath ) : BaseDump(filePath, soPath) {
}

/**
 * Virtual D'tor.
 */
DesDecDump::~DesDecDump() {}

/**
 * Dumps the output (based on input file or stdin)
 */
int DesDecDump::dump() {

    int status = 0;
    DesDecryptor decryptor;
    
    status = openInputFile( true );

    if( 0 == status ) {
        status = decryptor.init( _soPath, _filePath );
        
        if( 0 == status ) {
            status = decryptor.decrypt( _fp, stdout );
        }

        //close input file
        closeInputFile();
    }
    
    return status;

}

