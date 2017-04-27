/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include "cfilemgr.h"
#include "cconst.h"
#include "cexception.h"
#include "cutil.h"
#include "cfile.h"
#include "cdir.h"
#include "cautoptr.h"
#include "cifstream.h"
#include "cofstream.h"
#include "csha1data.h"
#include "cbitvector.h"
#include "cmutexlocker.h"
#include "ccert.h"

/*!
  Construts the File Manager with the given file path
*/
CFileMgr::CFileMgr()
{
  _iniP = NULL;
}

/*!
  Deletes the log
*/
CFileMgr::~CFileMgr() {}

/*!
  Initializes the file manager
*/
void CFileMgr::initialize(
  IServant *s, CIniParser *p,
  CLog *l)
{
  _servant = s; assert(s);
  _iniP = p; assert(p);
  _log = l; assert(l);

  initFilesDir();
  initFileIndex();
  initKwrdIndex();
  initSha1Index();
  initNameIndex();

  CFileMgr::_filesDir = _iniP->getHomePath(DIR_FILES);
}

void CFileMgr::reset(
  IServant *s, CIniParser *p,
  CLog *l)
{
  _servant = s; assert(s);
  _iniP = p; assert(p);
  _log = l; assert(l);

  initNameIndex();
  StringVector nVct = _nameFileMap.getKeys();

  for (uint i=0; i<nVct.size(); i++) {
    UIntVector fNumVct = _nameFileMap.get(nVct.at(i));

    for (uint j=0; j<fNumVct.size(); j++) {
      uint fNum = fNumVct.at(j);
      deleteIfExist(getFilePath(fNum, SUFFIX_CACHE));
      deleteIfExist(getFilePath(fNum, SUFFIX_DATA));
      deleteIfExist(getFilePath(fNum, SUFFIX_META));
      deleteIfExist(getFilePath(fNum, SUFFIX_CERT));
    }
  }

  deleteIfExist(_iniP->getHomePath(FILE_NAME_INDEX));
  deleteIfExist(_iniP->getHomePath(FILE_KWRD_INDEX));
  deleteIfExist(_iniP->getHomePath(FILE_SHA1_INDEX));
  deleteIfExist(_iniP->getHomePath(FILE_INDEX));  
}

/*!
  Deletes the file if path exists
*/
void CFileMgr::deleteIfExist(
  const string &path)
{
  if (CFile::exists(path)) {
    CFile::deleteFile(path);
  }
}

/*!
  Initialzes the files directory
*/
void CFileMgr::initFilesDir()
{
  string path = _iniP->getHomePath(DIR_FILES);
  if (! CDir::exists(path)) {
    CDir d;
    d.createDir(path);
  }
}

/*!
  Initializes the Name index
*/
void CFileMgr::initNameIndex()
{
  string path = _iniP->getHomePath(FILE_NAME_INDEX);
  if (CFile::exists(path)) {
    _nameFileMap.load(path);
  }
}

/*!
  Initializes the keyword index
*/
void CFileMgr::initKwrdIndex()
{
  string path = _iniP->getHomePath(FILE_KWRD_INDEX);
  if (CFile::exists(path)) {
    _kwrdFileMap.load(path);
  }
}

/*!
  Initializes the Sha1 index
*/
void CFileMgr::initSha1Index()
{
  string path = _iniP->getHomePath(FILE_SHA1_INDEX);
  if (CFile::exists(path)) {
    _sha1FileMap.load(path);
  }
}

/*!
  Initializes the file index
*/
void CFileMgr::initFileIndex()
{
  string path = _iniP->getHomePath(FILE_INDEX);
  if (CFile::exists(path)) {
    _fileIndexMap.load(path);
  }

  if (_fileIndexMap.size() <= 0) {
    setFileIndexVal(VAL_FILENUM, 0);
    setFileIndexVal(VAL_DATASIZE, 0);
    setFileIndexVal(VAL_CACHESIZE, 0);
  }
}

/*!
  Get file index val
*/
uint CFileMgr::getFileIndexVal(
  const string &field)
{
  UIntVector v = _fileIndexMap.get(field);
  assert(1 == v.size());
  
  return v.at(0);
}

/*!
  Set file index val
*/
void CFileMgr::setFileIndexVal(
  const string &field, uint val)
{
  UIntVector v;
  v.push_back(val);
  _fileIndexMap.set(field, v);
}

/*!
  Returns the file num
*/
uint CFileMgr::getFileNum()
{
  return getFileIndexVal(VAL_FILENUM);
}

/*!
  Increments the file number and stores in the file
*/
uint CFileMgr::incrFileNum()
{
  uint fileNum = getFileNum() + 1;
  setFileIndexVal(VAL_FILENUM, fileNum);
  return fileNum;
}

/*!
  Increments the file size
*/
void CFileMgr::incrFileSize(
  uint size, bool isCache)
{
  string field = (isCache) ? VAL_CACHESIZE :VAL_DATASIZE;

  uint fsize = getFileSize(isCache) + size;
  setFileIndexVal(field, fsize);
}

/*!
  Decrements the file size
*/
void CFileMgr::decrFileSize(
  uint size, bool isCache)
{
  string field = (isCache) ? VAL_CACHESIZE :VAL_DATASIZE;
  uint fsize = getFileSize(isCache);

  if (fsize < size) {
    string msg = ("size ")  + CUtil::getString(size);
    msg += string(" is greater than ") + CUtil::getString(fsize);
    C_THROW(msg);
  }
  fsize -= size;
  setFileIndexVal(field, fsize);  
}

/*!
  Returns the file size
*/
uint CFileMgr::getFileSize(bool isCache)
{
  string field = (isCache) ? VAL_CACHESIZE :VAL_DATASIZE;
  return getFileIndexVal(field);
}

/*!
  Stores the file into the disk
*/
void CFileMgr::storeFile(
  const string &fPath,
  const StringVector &v,
  const UCharBuffer &nonce,
  CMessageStore *mP)
{
  validateDataSpace(fPath);

  CFile f(fPath);  
  CSha1Data sha1;
  sha1.calculate(fPath);
  CBitVector bv;
  bv.setBloomFilter(v);
  
  CMetaData m;
  m.setFileName(CUtil::getFileName(fPath));
  m.setFileSize(f.size());
  m.setSha1(sha1.getData());
  m.setNonce(nonce);
  m.setKeywords(v);
  m.setBitVector(bv.getBuffer());

  storeFile(fPath, m, mP);
}

/*!
  Stores the file
*/
void CFileMgr::storeFile(
  const string &fPath,
  const CMetaData &m,
  CMessageStore *mP)
{
  assert(mP);
  uint fNum = incrFileNum();
  
  string oPath = getFilePath(fNum, SUFFIX_DATA);
  copyFile(fPath, oPath, NULL);

  CFile oFile(oPath);
  uint dataFileSize = oFile.size();
  mP->setFilePath(oPath);
  mP->setFileDataSize(dataFileSize);
  
  oPath = getFilePath(fNum, SUFFIX_META);
  m.save(oPath);
  
  mP->setMetaData(m.toBuffer());

  UCharBuffer b;
  oPath = getFilePath(fNum, SUFFIX_CERT);
  copyFile(_iniP->getHomePath(CCert::FILE_PUBLIC), oPath, &b);
  mP->setCertData(b);
  
  incrFileSize(dataFileSize, false);
  saveIndexes(m, fNum, true);
}

/*!
  Cache the file if enough space is available.
  Throws exception if caching is failed.
*/
void CFileMgr::cacheFile(const CMessageStore *m)
{
  storeCacheFile(
    m->getFilePath(),
    m->getMetaData(),
    m->getCertData());
}

/*!
  Cache the file if enough space is available.
  Throws exception if caching is failed.
*/
void CFileMgr::cacheFile(const CMessageGetRes *m)
{
  storeCacheFile(
    m->getFilePath(),
    m->getMetaData(),
    m->getCertData());
}

/*!
  Stores the file if enough space is available.
  Throws exception if caching is failed.
*/
void CFileMgr::storeFile(const CMessageGetRes *m)
{
  storeDataFile(
    m->getFilePath(),
    m->getMetaData(),
    m->getCertData());
}

/*!
  Caches the file given the meta data and
  certificate data.
*/
void CFileMgr::storeCacheFile(
  const string &fPath,
  const UCharBuffer &mData,
  const UCharBuffer &cData)
{
  CFile::errorIfNotExist(fPath);
  CFile file(fPath);
  uint fSize = static_cast<uint>(file.size());
  uint cacheSize = _iniP->getCacheSize() * 1024;

  if (cacheSize < fSize) {
    string msg = string("Unable to Cache. File size ");
    msg += CUtil::getString(fSize);
    msg += " is greater than total size ";
    msg += CUtil::getString(cacheSize);

    C_THROW(msg);
  }

  //if the file does not exist, then only store the file
  uint fNum = 0;
  CMetaData md = CMetaData::load(mData);
  CFileSpec fs(
      md.getFileName(),
      CUtil::getHexStr(md.getSha1()),
      CUtil::getHexStr(md.getNonce()));

  if (! isFilePresent(fs, fNum)) {
    uint spaceLeft = cacheSize - getFileSize(true);
    
    if (fSize > spaceLeft) {
      string msg("Performing LRU for file size ");
      msg += CUtil::getString(fSize);
      msg += string(", total space = ") + CUtil::getString(cacheSize);      
      msg += string(", space left = ") + CUtil::getString(spaceLeft);
      //printf("%s\n", msg.c_str()); //TODO - remove this
      _log->logDebug(msg);
      
      performLRU(fSize);
    }
    
    storeFile(fPath, mData, cData, true);
  }
}

void CFileMgr::storeDataFile(
  const string &fPath,
  const UCharBuffer &mData,
  const UCharBuffer &cData)
{
  validateDataSpace(fPath);

  CMetaData md = CMetaData::load(mData);
  CFileSpec fs(
      md.getFileName(),
      CUtil::getHexStr(md.getSha1()),
      CUtil::getHexStr(md.getNonce()));

  bool bDelete = false;
  bool bStore = true;
  uint fNum = 0;
  
  if (isFilePresent(fs, fNum)) {
    string path = getDataOrCacheFile(fNum);
    if (CUtil::endsWith(path, SUFFIX_CACHE)) {
      bDelete = true;
      
    } else {
      bStore = false;
    }
  }

  if (bStore) {
    storeFile(fPath, mData, cData, false);
  }

  if (bDelete) {
    deleteFiles(fNum);
  }
}

/*!
  Stores the file
*/
void CFileMgr::storeFile(
    const string &fPath,
    const UCharBuffer &mData,
    const UCharBuffer &cData,
    bool isCache)
{
  string suffix = (isCache) ? SUFFIX_CACHE : SUFFIX_DATA;

  CSha1Data sha1Data;
  sha1Data.calculate(fPath);
  CMetaData metaData = CMetaData::load(mData);

  if (metaData.getSha1() != sha1Data.getData()) {
    string msg = string("Sha1 value for file ") + fPath;
    msg += " does not match with the Sha1 value of the message.";
    C_THROW(msg);
  }

  CFile f(fPath);
  uint fSize = static_cast<uint>(f.size());
  
  uint fNum = incrFileNum();
  string oPath = getFilePath(fNum, suffix);
  copyFile(fPath, oPath, NULL);

  oPath = getFilePath(fNum, SUFFIX_META);
  saveToFile(oPath, mData);

  oPath = getFilePath(fNum, SUFFIX_CERT);
  saveToFile(oPath, cData);

  incrFileSize(fSize, isCache);
  saveIndexes(metaData, fNum, true);
}

/*!
  Returns true if the File is present with the given sha1
*/
bool CFileMgr::isFilePresent(
  const UCharBuffer &sha1)
{
  return _sha1FileMap.contains(CUtil::getHexStr(sha1));
}

/*!
  Returns true if the File ID is present
*/
bool CFileMgr::isFileIdPresent(
  const UCharBuffer &fileId)
{
  return _fileIdMap.contains(CUtil::getHexStr(fileId));
}

/*!
  Copies the file specified by the file id to
  the file identfied by path.
*/
void CFileMgr::getLocalFile(
  const UCharBuffer &fileId, const string &path)
{
  uint fNum = getFileNum(fileId);
  string inPath = getDataOrCacheFile(fNum);
  CFileMgr::copyFile(inPath, path, NULL);

  if (CUtil::endsWith(inPath, SUFFIX_CACHE)) {
      UCharBuffer mData = readFile(getFilePath(fNum, SUFFIX_META));
      UCharBuffer cData = readFile(getFilePath(fNum, SUFFIX_CERT));

      try {
        storeDataFile(inPath, mData, cData);
      } catch (CException &e) {
        _log->logError(e.toString());
        //printf("%s\n", e.toString().c_str());
      }
  }
}
  
/*!
  Gets the file given the file Id and fills
  the message.
*/
void CFileMgr::getFile(
  const UCharBuffer &fileId, CMessageGetRes *mP)
{
  uint fileNum = getFileNum(fileId);
  string path = getDataOrCacheFile(fileNum);

  CFile f(path);

  mP->setFilePath(path);
  mP->setTmpFile(false);
  mP->setFileDataSize(f.size());
  mP->setMetaData(readFile(getFilePath(fileNum, SUFFIX_META)));
  mP->setCertData(readFile(getFilePath(fileNum, SUFFIX_CERT)));
}

/*!
  Returns the signed filespec
*/
UCharBuffer CFileMgr::getSignedSpec(
  const CFileSpec &fs) const
{
  UCharBuffer specBuffer;

  string s;
  s  = CMetaData::FLD_FILENAME + CConst::NVP_SEP + fs.getFileName() + CConst::CRLF;
  s += CMetaData::FLD_SHA1 + CConst::NVP_SEP + fs.getSha1() + CConst::CRLF;
  s += CMetaData::FLD_NONCE + CConst::NVP_SEP + fs.getNonce() + CConst::CRLF;
    
  string inPath;
  string outPath;
  int inFd = 0;
  int outFd = 0;

  try {
    createTmpFile(inFd, inPath);
    ssize_t nBytes = write(inFd, s.c_str(), s.length());
    if(nBytes != (int)s.length()) {
      string msg = string("Unable to write ") + CUtil::getString(s.length());
      msg += string(" bytes to file ") + inPath;
      msg += string(". Bytes written ") + CUtil::getString(nBytes);
      C_THROW(msg);
    }

    createTmpFile(outFd, outPath);
    
    CCert::sign(_iniP->getHomeDir(), inPath, outPath);
    specBuffer = readFile(outPath);

    close(inFd);
    close(outFd);
    CFile::deleteFile(inPath);
    CFile::deleteFile(outPath);

  } catch (CException &e) {
    if (inFd > 0) {
      close(inFd);
      CFile::deleteFile(inPath);
    }
    if (outFd > 0) {
      close(outFd);
      CFile::deleteFile(outPath);
    }
    throw e;
  }

  return specBuffer;
}

/*!
  Deletes the file for the given filespec
*/
void CFileMgr::deleteFile(
  const UCharBuffer &fileSpec)
{
  uint fNum = 0;
  CFileSpec fs = CFileSpec::fromSignedSpec(fileSpec);
  
  if (isFilePresent(fs, fNum)) {
    string inPath;
    int inFd = 0;

    createTmpFile(inFd, inPath);
    try {
      ssize_t nBytes = write(inFd, &(fileSpec[0]), fileSpec.size());

      if(nBytes != (int)fileSpec.size()) {
        string msg = string("Unable to write ") + CUtil::getString(fileSpec.size());
        msg += string(" bytes to file ") + inPath;
        msg += string(". Bytes written ") + CUtil::getString(nBytes);
        C_THROW(msg);
      }

      string certPath = getFilePath(fNum, SUFFIX_CERT);
      if (CCert::verify(certPath, inPath)) {
        string msg = string("File deletion verified for ") + inPath;
        _log->logDebug(msg);

        deleteFiles(fNum);

      } else {
        string msg = string("File deletion NOT verified for ") + inPath;
        _log->logDebug(msg);
      }

      if (inFd > 0) {
        close(inFd);
        CFile::deleteFile(inPath);
      }
      
    } catch (CException &e) {
      _log->logError(e.toString());
      if (inFd > 0) {
        close(inFd);
        CFile::deleteFile(inPath);
      }
    }
  }
}

/*!
  Copies file from one path to other
*/
void CFileMgr::copyFile(
  const string &inPath, const string &outPath, UCharBuffer *b)
{
  CFile::errorIfNotExist(inPath);

  CFile iFile(inPath);
  CFile oFile(outPath);
  CIfStream istream(&iFile, true);
  COfStream ostream(&oFile, false);
  
  try {
    istream.open();
    ostream.open();

    int offset = 0;

    CAutoPtr<char> bufP;
    bufP = new char[FILE_BUF_SIZE];

    while(istream.isGood() && (! istream.isEof())) {
      int len = istream.read(bufP.get(), FILE_BUF_SIZE);

      if (len < 0) {
        string msg = string("Unable to read file ") + inPath;
        msg += string(". Read returned ") + CUtil::getString(len);
        C_THROW(msg);
      }

      ostream.write(bufP.get(), len);

      if (b) {
        b->resize(offset + len);
        memcpy( &((*b)[0]), bufP.get(), len);
        offset += len;
      }
    }
    istream.close();
    ostream.close();
    
  } catch (CException &e) {
    if (istream.isOpened()) {
      istream.close();
    }
    if (ostream.isOpened()) {
      ostream.close();
    }
  }
}

/*!
  Saves the content of the UCharBuffer to file
*/
void CFileMgr::saveToFile(
    const string &fPath,
    const UCharBuffer &b)
{
  CFile f(fPath);
  COfStream stream(&f, false);
  stream.open();

  try {
    const char *buf = reinterpret_cast<const char*>(&(b[0]));
    stream.write(buf, b.size());
    stream.close();
  } catch (CException &e) {
    stream.close();
    throw e;
  }
}

/*!
  Reads the file and saves in UCharBuffer
*/
UCharBuffer CFileMgr::readFile(const string &fPath)
{
  CFile::errorIfNotExist(fPath);
  CFile f(fPath);
  CIfStream stream(&f, false);
  stream.open();

  UCharBuffer b(f.size());

  try {
    if (stream.isGood() && ! stream.isEof()) {
      char *c = reinterpret_cast<char*>(&(b[0]));
      stream.read(c, b.size());
      stream.close();
    }
      
  } catch (CException &e) {
    stream.close();
    throw e;
  }

  return b;
}

/*!
  Returns the file id and file name of the temporary
  file created.
*/
void CFileMgr::createTmpFile(int &fd, string &fName)
{
  char cfile[] = "/tmp/svtmp_XXXXXX";
  fd = mkstemp(cfile);
  fName = cfile;

  if (-1 == fd) {
    C_THROW("Unable to create a temp file.");
  }
}


/*!
  saves the indexes for the given metadata
*/
void CFileMgr::saveIndexes(
  const CMetaData &m, uint fileNum, bool append)
{

  if (append) {
    _nameFileMap.insert(m.getFileName(), fileNum);
    _kwrdFileMap.insert(m.getBitVector(), fileNum);
    _sha1FileMap.insert(m.getSha1(), fileNum);
    
  } else {
    _nameFileMap.removeVal(m.getFileName(), fileNum);
    _kwrdFileMap.removeVal(m.getBitVector(), fileNum);
    _sha1FileMap.removeVal(m.getSha1(), fileNum);
  }

  _nameFileMap.save(_iniP->getHomePath(FILE_NAME_INDEX));
  _kwrdFileMap.save(_iniP->getHomePath(FILE_KWRD_INDEX));
  _sha1FileMap.save(_iniP->getHomePath(FILE_SHA1_INDEX));
  
  saveFileIndex();
}

/*!
  Saves the FileIndex data structure to file
*/
void CFileMgr::saveFileIndex()
{
  _fileIndexMap.save(_iniP->getHomePath(FILE_INDEX));
}

/*!
  Returns vector of meta data in response to search
*/
MetaDataVector CFileMgr::search(
  uint8_t type, const string &query)
{
  MetaDataVector mVec;

  if (CConst::SEARCH_FILENAME == type) {
    searchFile(query, mVec);

  } else if (CConst::SEARCH_SHA1 == type) {
    searchSha1(query, mVec);

  } else if (CConst::SEARCH_KWRDS == type) {
    searchKwrds(query, mVec);

  } else {
    string msg = "Invalid search option ";
    msg += CUtil::getString(type);
    C_THROW(msg);
  }

  return mVec;
}


/*!
  Returns vector of meta data in response to search
*/
void CFileMgr::searchFile(
  const string &name, MetaDataVector &mVec)
{
  if (_nameFileMap.contains(name)) {
    UIntVector fVec = _nameFileMap.get(name);
    getSearchMeta(fVec, mVec);
  }
}

/*!
  Returns vector of meta data in response to search
*/
void CFileMgr::searchSha1(
  const string &sha1, MetaDataVector &mVec)
{
  //check if sha1 input is valid
  CSha1Data::fromHexStr(sha1);

  if (_sha1FileMap.contains(sha1)) {
    UIntVector fVec = _sha1FileMap.get(sha1);
    getSearchMeta(fVec, mVec);
  }
}

/*!
  Returns vector of meta data in response to search
*/
void CFileMgr::searchKwrds(
  const string &kwrds, MetaDataVector &mVec)
{
  //construct a query vector for the given query
  StringVector qVct = CUtil::split(kwrds, CConst::SPACE_SEP);

  if (qVct.size() <=0) {
    string msg = "Invald kwrds query. No items found";
    C_THROW(msg);
  }

  UIntVector fVec;

  CBitVector qbv;
  qbv.setBloomFilter(qVct);
  UCharBuffer qb = qbv.getBuffer();

  StringVector kVct = _kwrdFileMap.getKeys();

  for (uint i=0; i<kVct.size(); i++) {
    string key = kVct.at(i);
    UCharBuffer kb = qbv & CBitVector::fromHexStr(key);

    if (qb == kb) {
      getKwrdIndex(qVct, _kwrdFileMap.get(key), fVec);
    }
  }

  if (fVec.size() > 0) {
    getSearchMeta(fVec, mVec);
  }
}


/*!
  Returns the meta data for the given file ids.
  Also inserts the search file id's in the member
  variable.
*/
void CFileMgr::getSearchMeta(
  const UIntVector &fVec, MetaDataVector &mVec)
{
  for (uint i=0; i<fVec.size(); i++) {
    string path = getFilePath(fVec.at(i), SUFFIX_META);
    if (CFile::exists(path)) {
      UCharBuffer uoid = _servant->getNewUoid(CConst::OBJ_TYPE_FILE);
      CMetaData m = CMetaData::load(path);
      m.setFileId(uoid);
      mVec.push_back(m);
      _fileIdMap.insert(uoid, fVec.at(i));
    }
  }
}

/*!
  Returns the file info for all stored files
*/
MetaDataVector CFileMgr::getAllMetaInfo()
{
  MetaDataVector mVct;
  StringVector nVct = _nameFileMap.getKeys();

  for (uint i=0; i<nVct.size(); i++) {
    UIntVector fNumVct = _nameFileMap.get(nVct.at(i));

    for (uint j=0; j<fNumVct.size(); j++) {
      string path = getFilePath(fNumVct.at(j), SUFFIX_META);
      CMetaData mData = CMetaData::load(path);
      mVct.push_back(mData);
    }
  }

  return mVct;
}


/*!
  Returns the keyword index for the given keywords.
  If the keywords are in the file, then it'll update the
  retVec with that filenum.
*/
void CFileMgr::getKwrdIndex(
  const StringVector &qVct,
  const UIntVector &fVec,
  UIntVector &retVec)
{
  for (uint i=0; i<fVec.size(); i++) {
    string path = getFilePath(fVec.at(i), SUFFIX_META);
    if (CFile::exists(path)) {
      CMetaData m = CMetaData::load(path);

      if (m.isAllKwrdsPresent(qVct, false)) {
        retVec.push_back(fVec.at(i));
      }
    }
  }
}

/*!
  Returns the filenum for the given fileid
*/
uint CFileMgr::getFileNum(const UCharBuffer &fileId)
{
  errorIfFileIdNotFound(fileId);

  UIntVector v = _fileIdMap.get(CUtil::getHexStr(fileId));
  if (1 != v.size()) {
    string msg = "File id size is not as expected (1). Expected = ";
    msg += CUtil::getString(v.size());
    C_THROW(msg);
  }

  return v.at(0);
}

/*!
  Returns true if the file is present for the given
  filespec.
*/
bool CFileMgr::isFilePresent(
  const CFileSpec &fs, uint &fNum)
{
  bool isPresent = false;

  if (_nameFileMap.contains(fs.getFileName()) &&
      _sha1FileMap.contains(fs.getSha1())) {

    UIntVector v = _nameFileMap.get(fs.getFileName());

    for (uint i=0; i<v.size(); i++) {
      
      string path = getFilePath(v.at(i), SUFFIX_META);
      if (CFile::exists(path)) {
        CMetaData m = CMetaData::load(path);

        if (CUtil::getHexStr(m.getNonce()) == fs.getNonce()) {
          isPresent = true;
          fNum = v.at(i);
          break;
        }
      }
    }
  }
  return isPresent;
}

/*!
  Deletes the files specified by the file number
  and updates the data structures
*/
void CFileMgr::deleteFiles(uint fNum)
{
  string fPath = getDataOrCacheFile(fNum);
  string mPath = getFilePath(fNum, SUFFIX_META);
  string cPath = getFilePath(fNum, SUFFIX_CERT);

  string msg = string("Deleting files ") + fPath;
  msg += string(", ") + mPath + string(" and ") + cPath;
  _log->logDebug(msg);
  //printf("%s\n", msg.c_str());
  
  CFile file(fPath);
  uint size = static_cast<uint>(file.size());
  CMetaData mData = CMetaData::load(mPath);
  bool isCache = CUtil::endsWith(fPath, SUFFIX_CACHE);
  decrFileSize(size, isCache);
  
  CFile::deleteFile(fPath);
  CFile::deleteFile(mPath);
  CFile::deleteFile(cPath);

  saveIndexes(mData, fNum, false);
}

/*!
  Throws exception if the fi
*/
void CFileMgr::errorIfFileIdNotFound(
  const UCharBuffer &fileId)
{
  string sId = CUtil::getHexStr(fileId);
  if (! _fileIdMap.contains(sId)) {
    string msg = string("File id ") + sId;
    msg += " is not found.";
    C_THROW(msg);
  }
}

/*!
  Returns data or cache file path for the
  given file num
*/
string CFileMgr::getDataOrCacheFile(uint fNum) const
{
  string path = getFilePath(fNum, SUFFIX_DATA);

  if (CFile::exists(path)) {
    return path;
  }

  path = getFilePath(fNum, SUFFIX_CACHE);
  if (CFile::exists(path)) {
    return path;
  }

  string msg = "Unable to find data or cache file for numer ";
  msg += CUtil::getString(fNum);
  C_THROW(msg);

  return path;  
}

/*!
  Returns the file path for the given file
*/
string CFileMgr::getFilePath(
  uint fNum, const string &suffix) const
{
  string file = CUtil::getString(fNum) + string(".") + suffix;
  string dir = _iniP->getHomePath(DIR_FILES);

  return CUtil::getPath(dir, file);
}

/*!
  Validates the data space. Throws exception
  if not enough space is available to store data.
*/
void CFileMgr::validateDataSpace(const string &inFile)
{
  CFile::errorIfNotExist(inFile);
  CFile f(inFile);
  uint size = (uint)f.size();
  uint fileDataSize = getFileSize(false);
  uint maxSize = _iniP->getPermSize() * 1024;

  if ((size + fileDataSize) > maxSize) {
    string msg = "No space left for storing file ";
    msg += inFile;
    msg += string(". file size = ") + CUtil::getString(size);
    msg += string(" current size = ") + CUtil::getString(fileDataSize);
    msg += string(" max size = ") + CUtil::getString(maxSize);
    C_THROW(msg);
  }
}

//----Code for LRU-------------------------------------------------------------
typedef int(*CompareFunc)(const void*, const void*);

/*!
  C function to perform file selection
*/
int file_select(const struct dirent *entry)
{
  if (NULL != entry->d_name) {
    string name = entry->d_name;
    if (CUtil::endsWith(name, CFileMgr::SUFFIX_CACHE)) {
      return 1;
    }
  }
  return 0;
}

/*!
  Returns the last access time of the file
*/
uint get_access_time(
  const char *path)
{
  assert(path);

  string sPath = CUtil::getPath(CFileMgr::_filesDir, path);
  struct stat st;
  int status = stat(sPath.c_str(), &st);
  
  if (0 != status) {
    string msg = string ("Unable to stat the file ") + path;
    C_THROW(msg);
  }
  return st.st_atime;
}

/*!
  Last access sorting  
*/
int last_access_sort(
  const struct dirent** a, const struct dirent** b)
{
  uint aTime = get_access_time((*a)->d_name);
  uint bTime = get_access_time((*b)->d_name);

  return aTime > bTime;
}

/*!
  ATTN: CODE REUSE.
  Modified for compiling in both Linux and solaris.
  Solaris does not support scandir method.
  Code Source:
  http://www.koders.com/c/fid67F09A5A42BB1F6B563434398602BF78C930F0C0.aspx?s=TV+Raman
*/

/*******************************************************************************
 *                                                                             *
 *                                   Viewmol                                   *
 *                                                                             *
 *                              S C A N D I R . C                              *
 *                                                                             *
 *                 Copyright (c) Joerg-R. Hill, December 2000                  *
 *                                                                             *
 *******************************************************************************/
int scandir_p(const char *dir, struct dirent ***namelist,
            int (*select)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **))
{
  DIR *d;
  struct dirent *entry;
  register int i=0;
  size_t entrysize;

  if ((d=opendir(dir)) == NULL)
     return(-1);

  *namelist=NULL;
  while ((entry=readdir(d)) != NULL)
  {
    if (select == NULL || (select != NULL && (*select)(entry)))
    {
      *namelist=(struct dirent **)realloc((void *)(*namelist),
               (size_t)((i+1)*sizeof(struct dirent *)));

      if (*namelist == NULL) return(-1);
      entrysize=sizeof(struct dirent)-sizeof(entry->d_name)+strlen(entry->d_name)+1;
      (*namelist)[i]=(struct dirent *)malloc(entrysize);
      if ((*namelist)[i] == NULL) return(-1);
      memcpy((*namelist)[i], entry, entrysize);
      i++;
    }
  }
  if (closedir(d)) return(-1);
  if (i == 0) return(-1);
  if (compar != NULL) {
    qsort((void *)(*namelist), (size_t)i, sizeof(struct dirent *), (CompareFunc)compar);
  }
  return(i);
}

/*!
  Returne the list of last accessed files
  sorted by timestamp
*/
StringVector CFileMgr::getLaCacheList()
{
  StringVector vctRet;
  struct dirent **files;
  string path = _iniP->getHomePath(DIR_FILES);

  int count = scandir_p(path.c_str(), &files, file_select, last_access_sort);

  for (int i=0; i<count; i++) {
    vctRet.push_back(files[i]->d_name);
    free(files[i]);
  }
  if (files) {
    free(files);
  }

  return vctRet;
}

/*!
  Performs the LRU operation for the given file size
*/
void CFileMgr::performLRU(uint tFileSize)
{
  uint curSize = tFileSize;
  StringVector vctF = getLaCacheList();

  for (uint i=0; i< vctF.size(); i++) {
    string fPath = CUtil::getPath(CFileMgr::_filesDir, vctF.at(i));
    CFile::errorIfNotExist(fPath);
    CFile f(fPath);
    uint size = f.size();
    
    string msg = string("performLRU: fpath = ") + fPath;
    //printf("%s\n", fPath.c_str());

    uint fNum = getFileNum(vctF.at(i));
    deleteFiles(fNum);

    if (size >= curSize) {
      break;
      
    } else {
      curSize -= size;
    }
  }
}

/*!
  Returns the filenum given the filename
*/
uint CFileMgr::getFileNum(
  const string &fileName)
{
  string name;
  string val;

  CUtil::getNvp(fileName, ".", name, val);
  int fNum = CUtil::getInteger<int>(name, true);
  return static_cast<uint>  (fNum);
}


//-----------------------------------------------------------------------------
string CFileMgr::_filesDir;

const string CFileMgr::DIR_FILES          = "files";
const string CFileMgr::FILE_KWRD_INDEX    = "kwrd_index";
const string CFileMgr::FILE_NAME_INDEX    = "name_index";
const string CFileMgr::FILE_SHA1_INDEX    = "sha1_index";
const string CFileMgr::FILE_INDEX         = "file_index";
const string CFileMgr::SUFFIX_DATA        = "data";
const string CFileMgr::SUFFIX_META        = "meta";
const string CFileMgr::SUFFIX_CACHE       = "cache";
const string CFileMgr::SUFFIX_CERT        = "cert";
const string CFileMgr::VAL_FILENUM        = "filenum";
const string CFileMgr::VAL_DATASIZE       = "datasize";
const string CFileMgr::VAL_CACHESIZE      = "cachesize";
