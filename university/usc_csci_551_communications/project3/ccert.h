/***************************************************************************
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CCERT_H
#define CCERT_H

#include "cglobal.h"

class CCert
{
public:
  ~CCert();

  static void reset(const string &certDir);
  static bool isCertExist(const string &certDir);
  static void genCert(const string &certDir);
  static void sign(const string &certDir, const string &inPath, const string &outPath);
  static bool verify(const string &pubPath, const string &filePath);
  

  static const string FILE_PUBLIC;
  
private:
  CCert();
  static string execCmd(const string &cmd);  

  static const string FILE_PRIVATE;
  static const string PATH_WHOAMI;
  static const string PATH_OPENSSL;
  static const string VERIFY_STR;
};

#endif //CCERT_H

