/**
 * Class for Base 64b encoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __BASE64DECDUMP_H__
#define __BASE64DECDUMP_H__

#ifdef WIN32
    #pragma warning( disable : 4786)
#endif


#include "basedump.h"
#include <map>
#include <openssl/bio.h>

typedef std::map<char, unsigned char> Base64DecDumpMap;

class Base64DecDump : public BaseDump{

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    Base64DecDump(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~Base64DecDump();

    /**
     * Return the instance of base64
     */
    static Base64DecDump * getInstance();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();

    /**
     * Verifies the input data. If the data is not valid, it will set the 
     * _bError flag to false
     *
     * @param buffer_ptr        Input Buffer
     * @param bufferLen         Length of the buffer
     */
    void verifyInputData(
            const char * buffer_ptr,
            unsigned int bufferLen );
private:

    /**
     * Creates Base 64 BIO
     *
     * @param bio               BIO to be created
     * @param buffer            Memory buffer
     * @param bufferLen         Length of the buffer
     *
     * @return      returns 0 if creation is successful
     */
    int createB64BIO( 
            BIO *& bio, 
            unsigned char * buffer,  
            unsigned int bufferLen ) const;

    /**
     * Creates output BIO
     *
     * @param bio               BIO to be created
     * @return      returns 0 if creation is successful
     */
    int createOutBIO( BIO *& bio ) const;

    /**
     * Deletes specied BIO
     *
     * @param bio               BIO to be deleted
     *
     * @return      returns 0 if deletion is successful
     */
    void deleteBIO( BIO *& bio ) const;

    /**
     * Sets the error message with invalid input
     */
    void setInvalidInputError() const;

    /**
     * Validates the Encoded data to see if it follows 
     * base-64 encoding standard. If = sign is present,
     * it has to be the last two.
     *
     * @param dataArray             Array of data
     * 
     */
    int validateEncData( const char * dataArray );

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

    //---------------------------------------------------

    static Base64DecDump * _instance;   //returns instance
    static const int DEC_CHUNK_MAX_LEN; //length of the maximum decoding chunk that'll be processed at once
    
    bool _bError;                       //Indicating Error in input file
    int _paddedCount;                   //Number of padded data in the base 64 file
    int _curBitPos;                     //Current bit position (number of bits processed)
    int _curIndex;                      //Current Index of the byte within decodeBuffer
    int _firstPadIndex;                 //Index of the first padded character
    char * _decBuffer;                  //Decoding Buffer
    Base64DecDumpMap _decMap;           //Map of character and encoded value
};

#endif //__BASE64DECDUMP_H__

