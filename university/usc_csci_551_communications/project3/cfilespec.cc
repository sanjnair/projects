/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cfilespec.h"
#include "cconst.h"
#include "cutil.h"
#include "csha1data.h"
#include "cmetadata.h"

/*!
  C'tor
*/
CFileSpec::CFileSpec(
  const string &f, const string &s, const string &n)
{
  _fileName = f;
  _sha1 = s;
  _nonce= n;
}

/*!
  D'tor
*/
CFileSpec::~CFileSpec() {}

/*!
  Returns the filename
*/
string CFileSpec::getFileName() const
{
  return _fileName;
}

/*!
  Returns the sha1
*/
string CFileSpec::getSha1() const
{
  return _sha1;
}

/*!
  Returns the nonce
*/
string CFileSpec::getNonce() const
{
  return _nonce;
}

/*!
  Constructs the Filespec from user input
*/
CFileSpec CFileSpec::fromUserInput(
  const string &input)
{
  StringVector vct = CUtil::split(input, CConst::SPACE_SEP);
  const uint MIN_SIZE = 4;

  if (vct.size() < MIN_SIZE) {
    string msg("Invalid delete command. Three args expected.");
    C_THROW(msg);
  }
  string fname;
  string sha1;
  string nonce;

  for (uint i=1; i<MIN_SIZE; i++) {
    string name;
    string val;

    CUtil::getNvp(vct.at(i), CConst::NVP_SEP, name, val);
    name = CUtil::trim(name);
    val = CUtil::trim(val);
  
    if (CMetaData::FLD_FILENAME == name) {
      fname = val;
      
    } else if (CMetaData::FLD_SHA1 == name) {
      sha1 = val;
      CSha1Data::fromHexStr(sha1);

    } else if (CMetaData::FLD_NONCE == name) {
      nonce = val;
      CSha1Data::fromHexStr(nonce);
      
    } else {
      string msg = string("Invalid value ") + name;
      C_THROW(msg);
    }
  }

  if ((fname.length() <= 0) || (sha1.length() <= 0) ||
      (nonce.length() <= 0)) {

    C_THROW("Not all required values are entered");      
  }
  
  CFileSpec fs(fname, sha1, nonce);
  return fs;
}

/*!
  Returns the Filespec object from the signed filespec data
*/
CFileSpec CFileSpec::fromSignedSpec(const UCharBuffer &b)
{
  const char *c = reinterpret_cast<const char*>(&(b[0]));
  string s(c, b.size());

  StringVector vc = CUtil::split(s, CConst::NEWLINE);
  string fname;
  string sha1;
  string nonce;
  bool secValid = false;
  

  for (uint i=0; i<vc.size(); i++) {
    string name;
    string val;
    string line = CUtil::trim(vc.at(i));

    if (CUtil::startsWith(line, SECTION_SEP)) {
      if (! secValid) {
        secValid = true;
      } else {
        break;
      }
    }    

    if (secValid) {
      if (CUtil::startsWith(line, CMetaData::FLD_FILENAME)) {
        CUtil::getNvp(line, CConst::NVP_SEP, name, val);
        fname = val;
      
      } else if (CUtil::startsWith(line, CMetaData::FLD_SHA1)) {
        CUtil::getNvp(line, CConst::NVP_SEP, name, val);
        sha1 = val;

      } else if (CUtil::startsWith(line, CMetaData::FLD_NONCE)) {
        CUtil::getNvp(line, CConst::NVP_SEP, name, val);
        nonce = val;
      }
    }
  }

  if ((fname.length() <= 0) || (sha1.length() <= 0) ||
      (nonce.length() <= 0)) {

    string msg = "Invalid filespec. Not all required values found. ";
    msg += CMetaData::FLD_FILENAME + string("=") + fname + string(", ");
    msg += CMetaData::FLD_SHA1 + string("=") + sha1 + string(", ");
    msg += CMetaData::FLD_NONCE + string("=") + nonce;
    
    C_THROW(msg);
  }
  
  CFileSpec fs(fname, sha1, nonce);
  return fs;  
}

//-----------------------------------------------------------------------------
const string CFileSpec::SECTION_SEP     = "----";
