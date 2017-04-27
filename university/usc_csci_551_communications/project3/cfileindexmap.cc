/***************************************************************************
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cfileindexmap.h"
#include "cconst.h"
#include "cmutexlocker.h"
#include "cexception.h"
#include "cutil.h"
#include "cifstream.h"
#include "cofstream.h"

/*!
  Constructs a neighbor map
*/
CFileIndexMap::CFileIndexMap() {}

/*!
  Deletes the neighbor map
*/
CFileIndexMap::~CFileIndexMap() {}

/*!
  Returns the size of the map
*/
size_t CFileIndexMap::size()
{
  CMutexLocker m(&_mutex);
  return _map.size();
}

/*!
  Inserts the key and val into the map
*/
void CFileIndexMap::insert(
  const UCharBuffer &key, uint n)
{
  insert(CUtil::getHexStr(key), n);
}

/*!
  Inserts the key and val into the map
*/
void CFileIndexMap::insert(
  const string &key, uint n)
{
  CMutexLocker m(&_mutex);
  insert_p(key, n);
}

void CFileIndexMap::removeVal(
  const UCharBuffer &key, uint n)
{
  removeVal(CUtil::getHexStr(key), n);
}

/*!
  Removes the val if present. If this is the
  only val, then key also gets removed
*/
void CFileIndexMap::removeVal(
  const string &key, uint n)
{
  CMutexLocker m(&_mutex);

  errorIfKeyNotExist(key);
  UIntVector &v = _map[key];

  for (uint i=0; i<v.size(); i++) {
    if (v.at(i) == n) {
      v.erase(v.begin() + i);
      break;
    }
  }

  if (v.size() <= 0) {
    remove_p(key);
  }
}

/*!
  Returns the value for the given key
*/
UIntVector CFileIndexMap::get(const string &key)
{
  CMutexLocker m(&_mutex);
  return get_p(key);
}

/*!
  Sets the data for the given key
*/
void CFileIndexMap::set(
  const string &key, const UIntVector &v)
{
  CMutexLocker m(&_mutex);
  set_p(key, v);
}

/*!
  Returns true if the mao contains the key
*/
bool CFileIndexMap::contains(const string &key)
{
  CMutexLocker m(&_mutex);
  return contains_p(key);
}

/*!
  Removes the the key
*/
void CFileIndexMap::remove(const string &key)
{
  CMutexLocker m(&_mutex);
  return remove_p(key);
}

/*!
  Clear the map
*/
void CFileIndexMap::clear()
{
  CMutexLocker m(&_mutex);
  return _map.clear();
}

/*!
  Returns the vector of keys
*/
StringVector CFileIndexMap::getKeys()
{
  CMutexLocker locker(&_mutex);
  
  StringVector keyVec;
  FileIndexMap::const_iterator it;

  for (it=_map.begin(); it != _map.end(); it++) {
    keyVec.push_back(it->first);
  }

  return keyVec;
}

/*!
  Saves the content of map to specified file
*/
void CFileIndexMap::save(const string &fPath)
{
  CMutexLocker locker(&_mutex);

  if (_map.size() > 0) {
    CFile oFile(fPath);
    COfStream ostream(&oFile, false);
    ostream.open();

    FileIndexMap::const_iterator it;
    try {
      for (it = _map.begin(); it != _map.end(); it++) {
        string line = it->first;
        line += CConst::NVP_SEP;
        line += joinNumList(it->second);
        line += CConst::CRLF;

        ostream.write(line);
      }
      ostream.close();
    } catch (CException &e) {
      ostream.close();
      throw e;
    }
  } else {
    if (CFile::exists(fPath)) {
      CFile::deleteFile(fPath);
    }
  }
}

/*!
  Loads the content of the File map
*/
void CFileIndexMap::load(const string &fPath)
{
  CMutexLocker locker(&_mutex);

  CFile::errorIfNotExist(fPath);
  _map.clear();

  string line;
  CFile f(fPath);
  CIfStream stream(&f, false);
  stream.open();

  string name;
  string val;

  try {
    while (stream.isGood() && ! stream.isEof()) {
      line = CUtil::trim(stream.readLine());

      if (line.length() >0) {
        CUtil::getNvp(line, CConst::NVP_SEP, name, val);

        if (contains_p(name)) {
          string msg = string("Name ") + name;
          msg += " already exist in the name map";
          C_THROW(msg);
        }
        UIntVector uVec = getNumList(val);
        _map[name] = uVec;
      }
    }
    stream.close();
  } catch (CException &e) {
    stream.close();
    throw e;
  }
}

/*!
  \internal
  Inserts the key and val into the map
*/
void CFileIndexMap::insert_p(
  const string &key, uint n)
{
  UIntVector v;
  if (contains_p(key)) {
    v = get_p(key);
  }
  v.push_back(n);
  _map[key] = v;
}

/*!
  \internal
  Returns the value specified by the key
*/
UIntVector CFileIndexMap::get_p(const string &key)
{
  errorIfKeyNotExist(key);
  return _map[key];
}

/*!
  \internal
  Sets the value for the speicified key
*/
void CFileIndexMap::set_p(
  const string &key, const UIntVector &v)
{
  _map[key] = v;
}

/*!
  \internal
  Removes the specified key from the map
*/
void CFileIndexMap::remove_p(const string &key)
{
  errorIfKeyNotExist(key);
  _map.erase(_map.find(key));
}

/*!
  \internal
  Returns true if map contains key
*/
bool CFileIndexMap::contains_p(const string &key)
{
  return (_map.find(key) != _map.end());   
}

/*!
  \internal
  Throw exception if the key does exist in the map
*/
void CFileIndexMap::errorIfKeyNotExist(const string &key)
{
  if (! contains_p(key)) {
    string msg = key + string(" not found in the map");
    C_THROW(msg);
  }
}

/*!
  Returns the num list given the string of file nums
*/
UIntVector CFileIndexMap::getNumList(const string &s)
{
  StringVector v = CUtil::split(s, CConst::COMMA_SEP);
  UIntVector uVec;

  for (uint i=0; i< v.size(); i++) {
    uVec.push_back(CUtil::getUInteger<uint>(v.at(i)));
  }

  return uVec;
}

/*!
  Join the num list and returns the string separated
*/
string CFileIndexMap::joinNumList(
  const UIntVector &v)
{
  string s;

  for (uint i=0; i<v.size(); i++) {
    s += CUtil::getString(v.at(i));
    if (i < (v.size() -1)) {
      s += CConst::COMMA_SEP;
    }
  }

  return s;
}
