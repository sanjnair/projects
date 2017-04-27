/**
 * Constant file for HW1
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
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
     * Returns the hex string for the given bytearray
     *
     * @param buffer                Buffer to write
     * @param bufferLen             Length of the buffer
     */
    static std::string getHexString( 
                    const unsigned char * buffer,
                    int bufferLen );

    /**
     * Opens the file. 
     * 
     * @param filePath              File Path
     * @param mode                  Mode
     * @param fp                    File pointer
     */
    static int openFile( 
                    const std::string & filePath,
                    const std::string & mode,
                    FILE *& fp );

    /**
     * Closes the file
     *
     * @param fp                    File pointer
     */
    static int closeFile( FILE * fp );

    /**
     * Writes the give string to the file pointed b file pointer.
     *
     * @param data                  Data
     * @param size                  Size
     * @param dataSize              Size of data
     * @param fp                    File pointer
     */
    static int HwUtil::writeToFile( 
                    const void * data,
                    unsigned int size,
                    unsigned int dataSize,
                    FILE * fp );
    
    /**
     * Writes the give string to the file pointed b file pointer.
	 *
	 * @param fp                    Valid file pointer
	 * @param str                   String to write
     */
    static int writeToFile( 
                    FILE * fp, 
                    const std::string & str);

    /**
     * Writes to file
	 *
     * @param fp                    File pointer
	 * @param buffer                Charracter buffer
     * @param bufferLen             Length of the buffer
     */
    static int writeToFile( 
                    FILE * fp,
                    const unsigned char * buffer, 
                    unsigned int bufferLen );

    /**
     * Writes to stdout
	 *
	 * @param str                   String to write
     */
    static int writeOutput( const std::string & str);

    /**
     * Writes to stdout and inserts a new line after the string.
	 *
	 * @param str                   String to write
     */
    static int writeOutputLn( const std::string & str);


    /**
     * Writes to stdout
	 *
	 * @param buffer                Charracter buffer
     * @param bufferLen             Length of the buffer
     */
    static int writeOutput( 
                    const unsigned char * buffer, 
                    unsigned int bufferLen );


    /**
     * Writes error to stderr. Program will append a newline after 
     * error being written.
	 *
	 * @param fp                    Valid file pointer
	 * @param str                   String to write
     * @param newLine               Append newline
     */
    static int writeError( 
                    const std::string & str, 
                    bool newLine = true );

    /**
     * Sets the last error message
     */
    static void setLastErrorMessage(const std::string & str );

    /**
     * Returns the last error message
     */
    static std::string getLastErrorMessage();

    /**
     * Returns the file size of the file
     *
     * @param fileName              Name of the file
     * @param size_out              File Size
     *
     * @return                      0, if successful.
     */
    static int getFileSize( 
                    const std::string & fileName, 
                    long & size_out );

    /**
     * Creates the buffer and returns the pointer to buffer.
     * If the buffer could not be created, this method returns
     * error code and buffer pointer will points to NULL.
     *
     * NOTE: Caller need to de-allocate the memory after use.
     *
     * @param size                  Size of the buffer
     * @param buffer_ptr            Reference to buffer pointer
     */
    static int createBuffer( int size, unsigned char *& buffer_ptr );

    /**
     * Deletes the buffer
     */
    static void deleteBuffer( unsigned char *& buffer_ptr );

    /**
     * Creates the buffer and returns the pointer to buffer.
     * If the buffer could not be created, this method returns
     * error code and buffer pointer will points to NULL.
     *
     * NOTE: Caller need to de-allocate the memory after use.
     *
     * @param size                  Size of the buffer
     * @param buffer_ptr            Reference to buffer pointer
     */
    static int createCharBuffer( int size, char *& buffer_ptr );

    /**
     * Deletes the buffer
     */
    static void deleteCharBuffer( char *& buffer_ptr );

    /**
     * Checks the read status of the file. If the read status is not
     * correct, this method will update the error message and returns 
     * error code.
     */
    static int checkFileReadStatus( FILE * fp );

    /**
     * Gets the value of the environment variable.
     * If value is null, then this method will return a non-zero return value.
     *
     * @param strName               Name of the environment variable
     * @param strValue              Valud of the environment variable
     */
    static int getEnvValue( 
                            const std::string & strName,
                            std::string & strValue );

private:
    /**
     * Constructor is private
     */
    HwUtil();

    static std::string _errorMsg;
};

#endif //__HWUTIL_H__

