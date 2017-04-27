/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CFILEINDEXMAP_H
#define CFILEINDEXMAP_H

#include "cglobal.h"
#include "cmutex.h"

class CFileIndexMap
{
public:
  CFileIndexMap();
  ~CFileIndexMap();

  size_t size();
  void insert(const UCharBuffer &key, uint n);
  void insert(const string &key, uint n);
  void removeVal(const string &key, uint n);
  void removeVal(const UCharBuffer &key, uint n);
  UIntVector get(const string &key);
  void set(const string &key, const UIntVector &v);
  bool contains(const string &key);
  void remove(const string &key);
  void clear();

  StringVector getKeys();
  
  void save(const string &fPath);
  void load(const string &fPath);

  static UIntVector getNumList(const string &s);
  static string joinNumList(const UIntVector &v);
  
private:
  void insert_p(const string &key, uint n);
  UIntVector get_p(const string &key);
  void set_p(const string &key, const UIntVector &v);  
  void remove_p(const string &key);
  bool contains_p(const string &key);
  void errorIfKeyNotExist(const string &key);  

  
  typedef std::map<string, UIntVector> FileIndexMap;
  FileIndexMap _map;
  CMutex _mutex;
};

#endif //CFILEINDEXMAP_H
