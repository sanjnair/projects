/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cstatuswriter.h"
#include "cexception.h"
#include "cutil.h"
#include "cconst.h"
#include "cmutexlocker.h"

/*!
  Construts the writer with the given file path
*/
CStatusWriter::CStatusWriter()
{
  _fileP = NULL;
  _streamP = NULL;
}

/*!
  Deletes the log
*/
CStatusWriter::~CStatusWriter()
{
  clear();
}

/*!
  Initializes the writer
*/
void CStatusWriter::initialize(
  const string &path, bool ns)
{
  clear();
  _path = path;

  _fileP = new CFile(_path);
  assert(_fileP);
  _streamP = new COfStream(_fileP, false);
  assert(_streamP);

  try {
    _streamP->open();

    if (ns) {
      writeToFile("V -t * -v 1.0a5", true);
    }

  } catch (CException &e) {
    clear();
  }
}

/*!
  Returns the file path
*/
string CStatusWriter::getFilePath() const
{
  return _path;
}

/*!
  Clears the writer
*/
void CStatusWriter::clear()
{
  if (NULL != _streamP) {
     if (_streamP->isOpened()) {
        _streamP->close();
     }
     delete _streamP;
     _streamP = NULL;
  }
  if (NULL != _fileP) {
    delete _fileP;
    _fileP = NULL;
  }
  _path.clear();
  _nodeMap.clear();
}

/*!
  Writes the node to file
*/
void CStatusWriter::writeNode(
    const string &host, uint16_t port,
    const NodeDataList &list)
{
  errorIfNotIntiailized();

  CMutexLocker locker(&_mutex);

  writeNodeToFile(port);
  if (list.size() > 0) {
    NodeDataList::const_iterator it;
    for (it = list.begin(); it != list.end(); it++) {
      uint16_t p = it->getPort();
      writeNodeToFile(p);

      writeLinkToFile(port, p);
    }
  }
}

/*!
  Writes meta data to file
*/
void CStatusWriter::writeMetaData(
    const string &host, uint16_t port,
    const MetaDataVector &vct)
{
  string s = host + string(":") + CUtil::getString(port) + string(" has ");

  if (0 == vct.size()) {
    s += "no file";
  } else if (1 == vct.size()) {
    s += "the following file";
  } else {
    s += "the following files";
  }
  s += CConst::CRLF;
  s += "--------------------------------------------------";
  s += CConst::CRLF;  

  if (vct.size() > 0) {
    for (uint i=0; i<vct.size(); i++) {
      s += vct.at(i).toString();
    }
  }
  s += CConst::CRLF; 
  writeToFile(s, false);
}

/*!
  Write a node to file
*/
void CStatusWriter::writeNodeToFile(uint16_t n)
{
  if (_nodeMap.find(n) == _nodeMap.end()) {
    _nodeMap[n] = n;

    string s = string("n -t * -s ") + CUtil::getString(n) +
               string(" -c red -i black");

    writeToFile(s, true);
  }
}

/*!
  Writes the link to the file
*/
void CStatusWriter::writeLinkToFile(uint16_t from, uint16_t to)
{
  if (from == to) {
    C_THROW("From value is same as to value\n");
  }

  uint16_t first = (from < to) ? from : to;
  uint16_t second = (from > to) ? from : to;
  string key = CUtil::getString(first) + string("_") + CUtil::getString(second);

  if (_linkMap.find(key) == _linkMap.end()) {
    _linkMap[key] = key;

    string s = string("l -t * -s ") + CUtil::getString(from) +
               string(" -d ") + CUtil::getString(to) +
               string(" -c blue");

    writeToFile(s, true);
  }
}


/*!
  Writes to the file
*/
void CStatusWriter::writeToFile(
  const string &s, bool newline)
{
  string str = s;
  if (newline) {
    str += CConst::NEWLINE;
  }
  _streamP->write(str.c_str(), str.length());
  _streamP->flush();
}

/*!
  Throws exception if the writer is not initialized
*/
void CStatusWriter::errorIfNotIntiailized() const
{
  if (NULL == _streamP) {
    C_THROW("Ns Writer is not initialzed");
  }
}
