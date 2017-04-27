/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cglobal.h"
#include <openssl/sha.h>
#include "cmetadata.h"
#include "cconst.h"
#include "cutil.h"
#include "cbitvector.h"
#include "cfile.h"
#include "cofstream.h"
#include "cifstream.h"
#include "csha1data.h"
#include "cfilemgr.h"

/*!
  Creates the File Metadata object
*/
CMetaData::CMetaData()
{
  _fileSize = 0;
}

/*!
  Deletes the JoinData object
*/
CMetaData::~CMetaData() {}

/*!
  Sets the filename
*/
void CMetaData::setFileName(const string &f)
{
  _fileName = f;
}

/*!
  Returns the filename
*/
string CMetaData::getFileName() const
{
  return _fileName;
}

/*!
  Sets the file size
*/
void CMetaData::setFileSize(long s)
{
  _fileSize = s;
}

/*!
  Sets the filesize
*/
long CMetaData::getFileSize() const
{
  return _fileSize;
}

/*!
  Sets the sha1
*/
void CMetaData::setSha1(const UCharBuffer &b)
{
  assert(b.size() == SHA_DIGEST_LENGTH);
  _sha1 = b;
}

/*!
  Returns the sha1
*/
UCharBuffer CMetaData::getSha1() const
{
  return _sha1;
}

/*!
  Sets the nonce  
*/
void CMetaData::setNonce(const UCharBuffer &b)
{
  assert(b.size() == SHA_DIGEST_LENGTH);
  _nonce = b;  
}

/*!
  Returns the nonce
*/
UCharBuffer CMetaData::getNonce() const
{
  return _nonce;
}

/*!
  Sets the keywords
*/
void CMetaData::setKeywords(const StringVector &v)
{
  _keywords = v;
}

/*!
  Return keywords
*/
StringVector CMetaData::getKeywords() const
{
  return _keywords;
}

/*!
  Sets the bit vector
*/
void CMetaData::setBitVector(const UCharBuffer &b)
{
  assert(b.size() == CBitVector::BYTE_SIZE);
  _bitVector = b;
}

/*!
  Returns the bit vector
*/
UCharBuffer CMetaData::getBitVector() const
{
  return _bitVector;
}

/*!
  Returns true if all the keywords present in the
  metadata
*/
bool CMetaData::isAllKwrdsPresent(
  const StringVector &v, bool cs) const
{
  bool found = false;

  for (uint i=0; i<v.size(); i++) {      
    found = isKwrdPresent(v.at(i), cs);

    if(! found) {
      break;
    }
  }  

  return found;
}

/*!
  Returns true if the keywor
*/
bool CMetaData::isKwrdPresent(
  const string &s, bool cs) const
{
  bool found = false;

  for (uint i=0; i<_keywords.size(); i++) {
    if (cs) {
      found = (s == _keywords.at(i));
    } else {
      found = (CUtil::toLower(s) == CUtil::toLower(_keywords.at(i)));
    }

    if (found) {
      break;
    }
  }

  return found;
}

/*!
  Sets the file id
*/
void CMetaData::setFileId(const UCharBuffer &b)
{
  _fileId = b;
}

/*!
  Gets the file id
*/
UCharBuffer CMetaData::getFileId() const
{
  return _fileId;
}
 
/*!
  Returns the string represenation of the metadata
*/
string CMetaData::toString() const
{
  string s;
  s  = FLD_HEADER + CConst::CRLF;
  s += getNvp(FLD_FILENAME, getFileName());
  s += getNvp(FLD_FILESIZE, CUtil::getString(getFileSize()));
  s += getNvp(FLD_SHA1, CUtil::getHexStr(getSha1()));
  s += getNvp(FLD_NONCE, CUtil::getHexStr(getNonce()));
  s += getNvpVector(FLD_KEYWORDS, getKeywords());
  s += getNvp(FLD_BITVECTOR, CUtil::getHexStr(getBitVector()));

  return s;
}

/*!
  Returns the string represenation of the metadata
*/
UCharBuffer CMetaData::toBuffer() const
{
  UCharBuffer b;
  string ms = toString();
  b.resize(ms.length());
  memcpy(&(b[0]), &(ms[0]), ms.length());
  
  return b;
}

/*!
  Returns the Nvp for the given name and value
*/
string CMetaData::getNvp(
  const string &n, const string &v)
{
  return (n + CConst::NVP_SEP + v + CConst::CRLF);
}

/*!
  Returns the Nvp for the given name and vector
*/
string CMetaData::getNvpVector(
  const string &n, const StringVector &v)
{
  string s;
  for (uint i=0; i<v.size(); i++) {
    s += v.at(i);
    if (i < v.size() -1) {
      s += KEYWORD_SEP;
    }
  }
  return getNvp(n, s);
}


/*!
  Saves the metadata to the specified file
*/
void CMetaData::save(const string &path) const
{
  if (CFile::exists(path)) {
    string msg = string("File ") + path + string(" exists");
    C_THROW(msg);
  }

  CFile f(path);
  COfStream stream(&f, true);
  stream.open();

  try {
    UCharBuffer b = toBuffer();
    const char *buf = reinterpret_cast<const char*>(&(b[0]));
    stream.write(buf, b.size());
    stream.close();
  } catch (CException &e) {
    stream.close();
    throw e;
  }
}

/*!
  Loads the metadata from the file
*/
CMetaData CMetaData::load(const string &path)
{
  UCharBuffer b = CFileMgr::readFile(path);
  return load(b);
}

/*!

  Loads the metadata from the buffer
*/
CMetaData CMetaData::load(const UCharBuffer &b)
{
  CMetaData mData;
  StringVector v = getFldsFromBuffer(b);

  for (uint i=0; i<v.size(); i++) {
    if (0 == i) {
      verifyHeader(v.at(0));

    } else {
      loadNvp(v.at(i), mData);
    }
  }
  return mData;
}


/*!
  Verifies the header
*/
void CMetaData::verifyHeader(const string &s)
{
  if (s != FLD_HEADER) {
    string msg = "Invalid metadata file. Invalid header ";
    msg += s;
    C_THROW(msg);
  }
}

/*!
  Loads the metadata from the given nvp
*/
void CMetaData::loadNvp(const string &s, CMetaData &m)
{
  string name;
  string val;
  
  CUtil::getNvp(s, CConst::NVP_SEP, name, val);

  if (FLD_FILENAME == name) {
    m.setFileName(val);
    
  } else if(FLD_FILESIZE == name) {
    m.setFileSize(CUtil::getInteger<long>(val));

  } else if(FLD_SHA1 == name) {
    m.setSha1(CSha1Data::fromHexStr(val));
  
  } else if(FLD_NONCE == name) {
    m.setNonce(CSha1Data::fromHexStr(val));
  
  } else if(FLD_KEYWORDS == name) {
    StringVector v = CUtil::split(val, KEYWORD_SEP);
    m.setKeywords(v);
  
  } else if(FLD_BITVECTOR == name) {
    CBitVector b = CBitVector::fromHexStr(val);
    m.setBitVector(b.getBuffer());
  }
}

/*!
  Returns the vector of fields from the buffer
*/
StringVector CMetaData::getFldsFromBuffer(const UCharBuffer &b)
{
  if (b.size() <= 0) {
    C_THROW("Invalid metadata buffer. 0 length");
  }

  const char *c = reinterpret_cast<const char*>(&(b[0]));
  string s(c, b.size());
  
  StringVector vct;
  string sData;
  uint from = 0;
  int index = 0;
  
  for (uint i=0; i<(NUM_FIELD_LINES-1); i++) {
    index = s.find(CConst::CRLF, from);

    if (index <= 0) {
      string msg = "Invalid metadata buffer. Data at index ";
      msg += CUtil::getString(i) + string(" is invalid. string index = ");
      msg += CUtil::getString(index)      ;
      C_THROW(msg);
    }
    sData = s.substr(from, index - from);
    vct.push_back(sData);
    from += index - from + CConst::CRLF.length();

  }

  //it may be possible that the CRLF is inside the bit vector
  if (from >= s.size()) {
    string msg = "Invalid metadata buffer. meta data from ";
    msg += CUtil::getString(from);
    msg += " is greater/equal than buffer size ";
    msg += CUtil::getString(s.size());
    C_THROW(msg);
  }

  sData = s.substr(from);
  //remove the CRLF from the string
  if (sData.length() <= CConst::CRLF.length()) {
    string msg = "Invalid metadata buffer (last field). length is ";
    msg += CUtil::getString(sData.length());
  }
  
  index = sData.length() - CConst::CRLF.length();
  sData = sData.substr(0, index);
  vct.push_back(sData);
  
  return vct;
}


//-----------------------------------------------------------------------------
const uint CMetaData::NUM_FIELD_LINES     = 7;
const string CMetaData::FLD_HEADER        = "[metadata]";
const string CMetaData::FLD_FILENAME      = "FileName";
const string CMetaData::FLD_FILESIZE      = "FileSize";
const string CMetaData::FLD_SHA1          = "SHA1";
const string CMetaData::FLD_NONCE         = "Nonce";
const string CMetaData::FLD_KEYWORDS      = "Keywords";
const string CMetaData::FLD_BITVECTOR     = "BitVector";
const string CMetaData::KEYWORD_SEP       = " ";
