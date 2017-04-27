/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CDIR_H
#define CDIR_H

#include "cglobal.h"
# define MAX_PATHSIZE 1024

class CDir {

public:
    CDir();
    CDir(const string &path);    
    ~CDir();

    string getPath() const;
    string getHomeDir() const;
    void createDir(const string &dirpath);
    void deleteDir(const string &dirpath);
    
    static bool exists(const string &path);
    static void errorIfNotExist(const string &path);

    static char separator;

private:
    string _path;
};

#endif //CDIR_H

