/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <iostream>
#include "unistd.h"
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
  Returns true if the file exists
*/
bool CFile::exists() const
{
  return CFile::exists(_path);
}

/*!
    Returns true if the file exists. Else false.
*/  
bool CFile::exists(const string &path)
{
  try {
    CFile f(path);
    f.size();
    return true;
  } catch (CException &) {}

  return false;
}

/*!
  Throws exception if the file does not exist
*/
void CFile::errorIfNotExist(const string &path)
{
  if (! CFile::exists(path)) {
    string msg = string("File '") + path + ("' does not exist.");
    C_THROW(msg);
  }
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

/*!
  Deletes the file
*/
void CFile::deleteFile(const string &path)
{
  CFile::errorIfNotExist(path);
  if (0 != unlink(path.c_str())) {
    string msg = string("Unable to delete file ") + path;
    C_THROW(msg);
  }
}
