/**
 * Class for DES encoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __DESENCDUMP_H__
#define __DESENCDUMP_H__

#include "basedump.h"

class DesEncDump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     * @param soPath            Shared libary Path
     */
    DesEncDump(
            const std::string filePath,
            const std::string soPath );

    /**
     * Virtual D'tor.
     */
    virtual ~DesEncDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
};

#endif //__DESENCDUMP_H__

