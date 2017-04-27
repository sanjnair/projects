/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

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

    struct stat buf;
    status = stat(_path.c_str(), &buf);

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

/*!
  Returns true, if it is a file
*/
bool CFile::isFile() const
{
    bool flag = false;

    struct stat buf;
    int status = stat(_path.c_str(), &buf);

    if (0 == status) {
      if(buf.st_mode & S_IFREG) {
         flag = true;
      }
    }

    return flag;
}
 
/*!
  Returns true, if it is a dir
*/
bool CFile::isDir() const
{
    bool flag = false;

    struct stat buf;
    int status = stat( _path.c_str(), &buf );

    if (0 == status) {
      if(buf.st_mode & S_IFDIR) {
         flag = true;
      }
    }

    return flag;
}

