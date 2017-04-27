#include "hwutil.h"
#include <iostream>
#include <sstream>

/**
 * Constructor is private
 */
HwUtil::HwUtil() { }

/**
 * Converts number to string
 */
std::string HwUtil::getString(int number) {

    std::stringstream converter;
    converter << number;
    
    return converter.str();
}

/**
 * Returns Hex string for the given character
 */
std::string HwUtil::getHexString(unsigned char c ) {

    std::string hexChar;
    char buffer[10];
    sprintf(buffer, "%02x", (unsigned char) c); 
   
    hexChar = buffer;

    return hexChar;
}

/**
 * Returns Hex string for the give number
 */
std::string HwUtil::getHexString(unsigned int c ) {

    std::string hexChar;
    char buffer[10];
    sprintf(buffer, "%06x", (unsigned int) c); 
   
    hexChar = buffer;

    return hexChar;
}

/**
 * Writes the give string to the file pointed b file pointer.
 *
 * @param fp                    Valid file pointer
 * @param str                   String to write
 */
void HwUtil::writeToFile( FILE * fp, const std::string & str) {
	
    fwrite(str.c_str(), sizeof(char), str.length(), fp );
}

/**
 * Writes error to stderr. Program will append a newline after 
 * error being written.
 *
 * @param fp                    Valid file pointer
 * @param str                   String to write
 */
void HwUtil::writeError(const std::string & str) {
    std::string lineStr = str + "\n";
    writeToFile(stderr, lineStr);
}

/**
 * Writes to stdout
 *
 * @param str                   String to write
 */
void HwUtil::writeOutput(const std::string & str) {
    writeToFile(stdout, str);
}

/**
 * Writes to stdout and inserts a new line after the string.
 *
 * @param str                   String to write
 */
void HwUtil::writeOutputLn(const std::string & str) {
    std::string lineStr = str + "\n";
    writeOutput(lineStr);
}

/**
 * Writes to stdout
 *
 * @param buffer                Charracter buffer
 * @param bufferLen             Length of the buffer
 */
void HwUtil::writeOutput( const unsigned char * buffer, unsigned int bufferLen ) {
    fwrite(buffer, sizeof(unsigned char), bufferLen, stdout );
}

/**
 * Sets the last error message
 */
void HwUtil::setLastErrorMessage(const std::string & str ) {
    _errorMsg = str;
}

/**
 * Returns the last error message
 */
std::string HwUtil::getLastErrorMessage() {
    return _errorMsg;
}

//static declaration
std::string HwUtil::_errorMsg = "";
