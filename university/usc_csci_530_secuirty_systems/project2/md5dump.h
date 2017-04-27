/**
 * Class for Sha-1 encoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __MD5DUMP_H__
#define __MD5DUMP_H__

#include "basedump.h"

class Md5Dump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    Md5Dump(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~Md5Dump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();

private:
};

#endif //__MD5DUMP_H__

