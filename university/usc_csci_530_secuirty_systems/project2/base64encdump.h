/**
 * Class for Base 64b encoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __BASE64ENCDUMP_H__
#define __BASE64ENCDUMP_H__

#include "basedump.h"
#include <openssl/bio.h>

class Base64EncDump : public BaseDump{

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    Base64EncDump(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~Base64EncDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();

private:

    /**
     * Creates and initializes BIO variables
     *
     * @return      returns 0 if creation is successful
     */
    int createBIO( BIO *& bio ) const;

    /**
     * Deletes BIO
     *
     * @return      returns 0 if deletion is successful
     */
    void deleteBIO( BIO *& bio ) const;
};

#endif //__BASE64ENCDUMP_H__

