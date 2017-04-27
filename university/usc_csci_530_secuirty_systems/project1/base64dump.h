/**
 * Base class for all dumps
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */

#ifndef __BASE64DUMP_H__
#define __BASE64DUMP_H__

#include "basedump.h"

class Base64Dump : public BaseDump{

public:
    
    /**
     * C'tor.
     *
     * @param filePath          File path
     */
    Base64Dump(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~Base64Dump();

protected:

};

#endif //__BASE64DUMP_H__

