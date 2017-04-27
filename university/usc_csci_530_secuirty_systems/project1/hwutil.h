/**
 * Constant file for HW1
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */

#ifndef __HWUTIL_H__
#define __HWUTIL_H__

#include <string>

class HwUtil {

public:

    /**
     * Converts number to string
	 *
	 * @param number                number
     */
    static std::string getString(int number);

    /**
     * Returns Hex string for the given character
	 *
	 * @param c                     Character 
     */
    static std::string getHexString(unsigned char c);

    /**
     * Returns Hex string for the given character
	 *
	 * @param c                     Character 
     */
    static std::string getHexString(unsigned int c);
    
    /**
     * Writes the give string to the file pointed b file pointer.
	 *
	 * @param fp                    Valid file pointer
	 * @param str                   String to write
     */
    static void writeToFile( FILE * fp, const std::string & str);
    
    /**
     * Writes error to stderr. Program will append a newline after 
     * error being written.
	 *
	 * @param fp                    Valid file pointer
	 * @param str                   String to write
     */
    static void writeError( const std::string & str);

    /**
     * Writes to stdout
	 *
	 * @param str                   String to write
     */
    static void writeOutput( const std::string & str);

    /**
     * Writes to stdout and inserts a new line after the string.
	 *
	 * @param str                   String to write
     */
    static void writeOutputLn( const std::string & str);

    /**
     * Writes to stdout
	 *
	 * @param buffer                Charracter buffer
     * @param bufferLen             Length of the buffer
     */
    static void writeOutput( const unsigned char * buffer, unsigned int bufferLen );

    /**
     * Sets the last error message
     */
    static void setLastErrorMessage(const std::string & str );

    /**
     * Returns the last error message
     */
    static std::string getLastErrorMessage();

private:
    /**
     * Constructor is private
     */
    HwUtil();

    static std::string _errorMsg;
};

#endif //__HWUTIL_H__

