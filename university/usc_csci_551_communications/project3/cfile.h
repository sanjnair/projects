/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CFILE_H
#define CFILE_H

#include "cglobal.h"

class CFile {

public:
    CFile(const string &path);
    ~CFile();

    string getName() const;
    long size() const;
    bool isFile() const;
    bool isDir() const;
    bool exists() const;

    static bool exists(const string &path);
    static void errorIfNotExist(const string &path);
    static void deleteFile(const string &path);

private:
    string _path;
};

#endif //CFILE_H

