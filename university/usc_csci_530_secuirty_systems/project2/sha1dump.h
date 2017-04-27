/**
 * Class for Sha-1 encoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __SHA1DUMP_H__
#define __SHA1DUMP_H__

#include "basedump.h"

class Sha1Dump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    Sha1Dump(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~Sha1Dump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
    
private:
};

#endif //__SHA1DUMP_H__

