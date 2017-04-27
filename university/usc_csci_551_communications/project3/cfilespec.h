/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CFILESPEC_H
#define CFILESPEC_H

#include "cglobal.h"

class CFileSpec
{
public:
  CFileSpec(const string &f, const string &s, const string &n);
  ~CFileSpec();

  string getFileName() const;
  string getSha1() const;
  string getNonce() const;
  
  static CFileSpec fromUserInput(const string &input);
  static CFileSpec fromSignedSpec(const UCharBuffer &b);
  
private:
  string _fileName;
  string _sha1;
  string _nonce;

  static const string SECTION_SEP;
};

#endif


