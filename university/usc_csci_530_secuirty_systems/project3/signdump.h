/**
 * Class for Sign Dump
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __SIGNDUMP_H__
#define __SIGNDUMP_H__

#include "basedump.h"


class SignDump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    SignDump( const std::string filePath, const std::string soPath );

    /**
     * Virtual D'tor.
     */
    virtual ~SignDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
    
private:
    
    /**
     * Gets the signed SHA-1 signature for the file.
     *
     * @param strSign           Signed string
     */
    int getSignedStr( std::string & strSign ) const;
};

#endif //__SIGNDUMP_H__

