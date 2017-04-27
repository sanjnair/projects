#include <iostream>
#include <sys/stat.h>
#include "cfile.h"
#include "cexception.h"


/**
 * Creates CFile for the given file name
 */
CFile::CFile(const string &path) : _path(path) {}

/*!
    Destroys CFile object
*/
CFile::~CFile() {}

/*!
    Returns the file name
*/
string CFile::getName() const
{
    return _path;
}

/*!
    Returns the size of the file
*/
long CFile::size() const
{
    int status = 0;

    #ifdef WIN32
        struct _stat buf;
        status = _stat( _path.c_str(), &buf );
    #else
        struct stat buf;
        status = stat( _path.c_str(), &buf );
    #endif

        if (0 != status) {
            string msg("Unable to get the size of the file ");
            msg += _path;
            C_THROW(msg);
        }

    return buf.st_size;
}

/*!
    Returns true if the file exists. Else false.
*/  
bool CFile::exists(const string &name)
{

    try {
        CFile f(name);
        f.size();
        return true;

    } catch (CException &) {}

    return false;
}   
