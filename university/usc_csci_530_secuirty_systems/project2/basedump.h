/**
 * Base class for all dumps
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */

#ifndef __BASEDUMP_H__
#define __BASEDUMP_H__

#include <stdio.h>
#include <string>
#include <assert.h>

class BaseDump {

public:
    
    /**
     * C'tor.
     *
     * @param filePath          File path
     */
    BaseDump(std::string filePath);

    /**
     * Virtual D'tor.
     */
    virtual ~BaseDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump() = 0;

protected:
    
    /**
     * Opens the input file. If the file path is valid and open is successful,
     * this methid creates a valid file pointer and keeps it. In this case, the
     * return code will be 0. Otherwise, return code will be non-zero.
     * 
     */
    virtual int openInputFile();

    /**
     * Opens the input file. If the file path is valid and open is successful,
     * this methid creates a valid file pointer and keeps it. In this case, the
     * return code will be 0. Otherwise, return code will be non-zero.
     * 
     */
    virtual int closeInputFile();

    /**
     * Checks the read status of the file. If the read status is not
     * correct, this method will update the error message and returns 
     * error code.
     */
    virtual int checkFileReadStatus() const;

    std::string _filePath;
    FILE * _fp;
};

#endif //__BASEDUMP_H__

