/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CMETADATA_H
#define CMETADATA_H

#include "cglobal.h"

class CMetaData
{
public:
  CMetaData();
  ~CMetaData();

  void setFileName(const string &f);
  string getFileName() const;
  void setFileSize(long s);
  long getFileSize() const;
  void setSha1(const UCharBuffer &b);
  UCharBuffer getSha1() const;
  void setNonce(const UCharBuffer &b);
  UCharBuffer getNonce() const;
  void setKeywords(const StringVector &v);
  StringVector getKeywords() const;
  void setBitVector(const UCharBuffer &b);
  UCharBuffer getBitVector() const;
  bool isAllKwrdsPresent(const StringVector &v, bool cs) const;
  bool isKwrdPresent(const string &s, bool cs) const;  
  
  void setFileId(const UCharBuffer &b);
  UCharBuffer getFileId() const;

  string toString() const;
  UCharBuffer toBuffer() const;

  void save(const string &path) const;
  static CMetaData load(const string &path);
  static CMetaData load(const UCharBuffer &b);  
  
private:
  static string getNvp(const string &n, const string &v);
  static string getNvpVector(const string &n, const StringVector &v);
  static void verifyHeader(const string &s);
  static void loadNvp(const string &s, CMetaData &m);
  static StringVector getFldsFromBuffer(const UCharBuffer &b);
  
  string _fileName;
  long _fileSize;
  UCharBuffer _sha1;
  UCharBuffer _nonce;
  StringVector _keywords;
  UCharBuffer _bitVector;
  UCharBuffer _fileId;

public:
  static const string FLD_HEADER;
  static const string FLD_FILENAME;
  static const string FLD_FILESIZE;
  static const string FLD_SHA1;
  static const string FLD_NONCE;
  static const string FLD_KEYWORDS;
  static const string FLD_BITVECTOR;
  
private:  
  static const uint NUM_FIELD_LINES;
  static const string KEYWORD_SEP;
};

typedef std::vector<CMetaData> MetaDataVector;

#endif


