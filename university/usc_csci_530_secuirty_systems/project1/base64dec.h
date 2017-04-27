/**
 * Class for Base 64b encoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __BASE64DEC_H__
#define __BASE64DEC_H__

#include "base64dump.h"

#ifdef WIN32
    #pragma warning( disable : 4786)
#endif
#include <map>

typedef std::map<char, unsigned char> Base64DecMap;

class Base64Dec : public Base64Dump{

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    Base64Dec(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~Base64Dec();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();

private:

    /**
     * Performs the base-encodingdump operation.
     *
     * @param buffer_ptr        Pointer to buffer
     */
    int performBase64Dec( const unsigned char *buffer_ptr );

    /**
     * Returns the 3 decoded characters for the 4 bytes of
     * encoded data.
     *
     * @param valueArray        Array where the bytes are stored
     * @param padLen            Length of the padded items
     */
    std::string getDecodedString( 
                    const unsigned char * valueArray, 
                    int padLen ) const;

    /**
     * Validates the Encoded data to see if it follows 
     * base-64 encoding standard. If = sign is present,
     * it has to be the last two.
     *
     * @param dataArray             Array of data
     * 
     */
    int validateEncData( const unsigned char * dataArray );

    /**
     * Outputs the decoded data for the given 4 bytes of encoded data.
     *
     * @param dataArray             Array of data
     */
    int outputDecodedData( const unsigned char * dataArray );

    /**
     * Initializes the decoding map
     */
    void initDecMap();

    /**
     * Returns true, if the map contains the given key
     *
     * @param key                   Key
     */
    bool isMapHasKey(unsigned char key) const;

    /**
     * Sets the error message with invalid input
     */
    void setInvalidInputError() const;

    static const int DEC_CHUNK_MAX_LEN;   //length of the maximum decoding chunk that'll be processed at once
    
    Base64DecMap _decMap;   //Map of character and encoded value
    int _paddedCount;       //Number of padded data in the base 64 file
    int _curBitPos;         //Current bit position (number of bits processed)
    
};

#endif //__BASE64DEC_H__

