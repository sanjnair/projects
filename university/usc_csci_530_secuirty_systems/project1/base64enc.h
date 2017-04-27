/**
 * Class for Base 64b encoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __BASE64ENC_H__
#define __BASE64ENC_H__

#include "base64dump.h"

#ifdef WIN32
    #pragma warning( disable : 4786)
#endif
#include <map>
typedef std::map<unsigned char, char> Base64EncMap;

class Base64Enc : public Base64Dump{

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    Base64Enc(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~Base64Enc();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();

private:

    /**
     * Performs the base-encodingdump operation.
     *
     * @param buffer_ptr        Pointer to buffer
     * @param bufferLen         Length of the buffer
     * @param strLine           String line (reference);
     */
    int performBase64Enc( 
            const unsigned char *buffer_ptr, 
            int bufferLen,
            std::string & strLine );

    /**
     * Returns the 4 ASCII characters for the 3 bytes 
     *
     * @param valueArray        Array where the bytes are stored
     * @param padLen            Length of the padded items
     */
    std::string getEncodedString( 
                    const unsigned char * buffer_ptr, 
                    int padLen ) const;

    /**
     * Returns the Base 64 encoding character for given value
     */
    char getBase64Char(unsigned char value) const;

    /**
     * Initializes the encoding map
     */
    void initEncMap();

    static const int ENC_CHUNK_MAX_LEN;   //length of the maximum encoding chunk that'll be processed at once    
    Base64EncMap _encMap;
};

#endif //__BASE64ENC_H__

