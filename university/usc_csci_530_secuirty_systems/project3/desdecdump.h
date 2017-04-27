/**
 * Class for DES decoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __DESDECDUMP_H__
#define __DESDECDUMP_H__

#include "basedump.h"

class DesDecDump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     * @param soPath            Shared libary Path
     */
    DesDecDump(
            const std::string filePath,
            const std::string soPath );
        
    /**
     * Virtual D'tor.
     */
    virtual ~DesDecDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
};

#endif //__DESDECDUMP_H__

