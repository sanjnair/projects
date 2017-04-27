#include "desencdump.h"
#include "desencryptor.h"

/**
 * C'tor.
 *
 * @param filePath          File path
 * @param soPath            Shared libary Path
 */
DesEncDump::DesEncDump( 
                const std::string filePath,
                const std::string soPath ) : BaseDump(filePath, soPath) {
    
}

/**
 * Virtual D'tor.
 */
DesEncDump::~DesEncDump() {}

/**
 * Dumps the output (based on input file or stdin)
 */
int DesEncDump::dump() {

    int status = 0;
    DesEncryptor encryptor;
    
    status = openInputFile( true );

    if( 0 == status ) {
        status = encryptor.init( _soPath, _filePath );
        
        if( 0 == status ) {
            status = encryptor.encrypt( _fp, stdout );
        }

        //close input file
        closeInputFile();
    }
    
    return status;
}

