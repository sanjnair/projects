/**
 * File operations
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CFILE_H__
#define __CFILE_H__

#include "cglobal.h"

class CFile {

public:
    CFile(const string &path);
    ~CFile();

    string getName() const;
    long size() const;

    static bool exists(const string &name);
    
private:
    string _path;
};

#endif //__CFILE_H__

