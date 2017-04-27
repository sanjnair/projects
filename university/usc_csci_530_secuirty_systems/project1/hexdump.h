/**
 * Class for hexdumnp
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */

#ifndef __HEXDUMP_H__
#define __HEXDUMP_H__

#include "basedump.h"

class HexDump : public BaseDump{

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    HexDump(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~HexDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();

private:
    
    /**
     * Performs the hex dump operation.
     *
     * @param buffer_ptr        Pointer to buffer
     * @param bufferLen         Length of the buffer
     */
    int performHexDump( unsigned char *buffer_ptr, int bufferLen );

    static const int HEX_CHUNK_MAX_LEN;   //length of the maximum data chunk that'll be processed at once
    unsigned int _hexLineValue;

};

#endif //__HEXDUMP_H__

