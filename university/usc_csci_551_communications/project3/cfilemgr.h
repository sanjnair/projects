/***************************************************************************
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CFILEMGR_H
#define CFILEMGR_H

#include "cglobal.h"
#include "ciniparser.h"
#include "cmetadata.h"
#include "cmutex.h"
#include "cmessage.h"
#include "ciservant.h"
#include "cfileindexmap.h"
#include "clog.h"
#include "cfilespec.h"

class CFileMgr
{
public:
  CFileMgr();
  ~CFileMgr();

  void initialize(
    IServant *s, CIniParser *p,
    CLog *l);
  
  void reset(
    IServant *s, CIniParser *p,
    CLog *l);

  void storeFile(
    const string &fPath,
    const StringVector &v,
    const UCharBuffer &nonce,
    CMessageStore *mP);

  void cacheFile(const CMessageStore *m);
  void cacheFile(const CMessageGetRes *m);
  void storeFile(const CMessageGetRes *m);
      
  bool isFilePresent(const UCharBuffer &sha1);
  bool isFileIdPresent(const UCharBuffer &fileId);
  void getLocalFile(const UCharBuffer &fileId, const string &path);
  void getFile(const UCharBuffer &fileId, CMessageGetRes *mP);
  MetaDataVector search(uint8_t type, const string &query);
  MetaDataVector getAllMetaInfo();

  UCharBuffer getSignedSpec(const CFileSpec &fs) const;
  void deleteFile(const UCharBuffer &fileSpec);
    
  static void copyFile(
    const string &inPath,
    const string &outPath,
    UCharBuffer *b = NULL);

  static void saveToFile(
    const string &fPath,
    const UCharBuffer &b);

  static UCharBuffer readFile(const string &fPath);
  static void createTmpFile(int &fd, string &fPath);
  
private:
  void deleteIfExist(const string &path);
  
  void initFilesDir();
  void initKwrdIndex();
  void initSha1Index();
  void initNameIndex();
  void initFileIndex();

  void saveIndexes(const CMetaData &m, uint fileNum, bool append);
  void saveFileIndex();

  uint getFileIndexVal(const string &field);
  void setFileIndexVal(const string &field, uint val);
  
  uint getFileNum();
  uint incrFileNum();

  void incrFileSize(uint size, bool isCache);
  void decrFileSize(uint size, bool isCache);
  uint getFileSize(bool isCache);  
  
  void storeFile(
    const string &fPath,
    const CMetaData &m,
    CMessageStore *mP);

  void storeCacheFile(
    const string &fPath,
    const UCharBuffer &mData,
    const UCharBuffer &cData);
  
  void storeDataFile(
    const string &fPath,
    const UCharBuffer &mData,
    const UCharBuffer &cData);  

  void storeFile(
    const string &fPath,
    const UCharBuffer &mData,
    const UCharBuffer &cData,
    bool isCache);

  void searchFile(
    const string &name,
    MetaDataVector &mVec);
    
  void searchSha1(
    const string &sha1,
    MetaDataVector &mVec);
    
  void searchKwrds(
    const string &kwrds,
    MetaDataVector &mVec);
    
  void getSearchMeta(
    const UIntVector &fVec,
    MetaDataVector &mVec);
    
  void getKwrdIndex(
    const StringVector &qVct,
    const UIntVector &fVec,
    UIntVector &retVec);

  uint getFileNum(const UCharBuffer &fileId);
  bool isFilePresent(const CFileSpec &fs, uint &fNum);
  
  void errorIfFileIdNotFound(const UCharBuffer &fileId);
  
  string getDataOrCacheFile(uint fNum) const;
  string getFilePath(uint fNum, const string &suffix) const;
  void validateDataSpace(const string &inFile);
  void deleteFiles(uint fNum);

  //--LRU methods
  void performLRU(uint tFileSize);
  StringVector getLaCacheList();
  uint getFileNum(const string &fileName);

  //---------------------------------------------------------------------------
  CFileIndexMap _nameFileMap;
  CFileIndexMap _kwrdFileMap;
  CFileIndexMap _sha1FileMap;
  CFileIndexMap _fileIndexMap;
  CFileIndexMap _fileIdMap;

  IServant *_servant;
  CIniParser *_iniP;
  CLog *_log;  
  CMutex _mutex;

public:
  static string _filesDir;
  
  static const string DIR_FILES;
  static const string FILE_KWRD_INDEX;
  static const string FILE_NAME_INDEX;
  static const string FILE_SHA1_INDEX;
  static const string FILE_INDEX;
  static const string FILE_SIZE_INDEX;
  static const string SUFFIX_DATA;
  static const string SUFFIX_META;
  static const string SUFFIX_CACHE;  
  static const string SUFFIX_CERT;
  static const string VAL_FILENUM;
  static const string VAL_DATASIZE;
  static const string VAL_CACHESIZE;
};

#endif //CFILEMGR_H

