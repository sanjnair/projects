/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <unistd.h>
#include <sys/stat.h>
#include "cdir.h"
#include "cfile.h"
#include "cexception.h"

/*!
  Creates empty dir
*/
CDir::CDir() {}

/*!
  Creates the directory with the path
*/
CDir::CDir(const string &path) : _path(path) {}

/*!
  Deletes the directory
*/
CDir::~CDir() {}

/*!
  Returns true if the directory exists
*/
bool CDir::exists(const string &path)
{
  CFile f(path);
  return (f.exists() && f.isDir());
}

/*!
  Throws exception if the dir does not exist
*/
void CDir::errorIfNotExist(const string &path)
{
  if (! CDir::exists(path)) {
    string msg = string("Folder '") + path + ("' does not exist.");
    C_THROW(msg);
  }
}

/*!
  Returns the directory path.
  If the path variable is null, it will
  return the current working dir
*/
string CDir::getPath() const
{
  if (_path.length() > 0) {
    return _path;
  }

  struct stat buf;
  if (0 != ::stat(".", &buf)) {
    string msg = string("stat call failed");
    C_THROW(msg);    
  }

  char pathBuf[MAX_PATHSIZE+1];
  if (! ::getcwd(pathBuf, MAX_PATHSIZE)) {
    string msg = string("getCwd call failed");
    C_THROW(msg);
  }

  return string(pathBuf);
}

/*!
  Returns the home directory
*/
string CDir::getHomeDir() const
{
  string s = getenv("HOME");
  if (s.length() <= 0) {
    C_THROW("Unable to get the user's home directory");
  }
  return s;
}

/*!
  Creates the directory
*/
void CDir::createDir(const string &dirpath)
{
  if (! CDir::exists(dirpath)) {
    if (0 != ::mkdir(dirpath.c_str(), 0777)) {
      string msg = string("unable to create directory ") + dirpath;
      C_THROW(msg);
    }
  }
}

/*!
  Deletes the directory
*/
void CDir::deleteDir(const string &dirpath)
{
  if (CDir::exists(dirpath)) {
    if (0 != ::rmdir(dirpath.c_str())) {
      string msg = string("unable to delete directory ") + dirpath;
      C_THROW(msg);
    }
  }
}

//-----------------------------------------------------------------------------
char CDir::separator    = '/';


