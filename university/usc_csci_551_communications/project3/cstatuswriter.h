/***************************************************************************
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CSTATUSWRITER_H
#define CSTATUSWRITER_H

#include "cfile.h"
#include "cofstream.h"
#include "cglobal.h"
#include "cnodedata.h"
#include "cmetadata.h"
#include "cmutex.h"

class CStatusWriter
{
public:

  CStatusWriter();
  ~CStatusWriter();

  void initialize(const string &file, bool ns);
  void clear();
  string getFilePath() const;

  void writeNode(
    const string &host, uint16_t port,
    const NodeDataList &list);

  void writeMetaData(
    const string &host, uint16_t port,
    const MetaDataVector &vct);

private:
  void writeToFile(const string &s, bool newline);
  void writeNodeToFile(uint16_t n);
  void writeLinkToFile(uint16_t from, uint16_t to);
  void errorIfNotIntiailized() const;

  CFile *_fileP;
  COfStream *_streamP;
  string _path;
  CMutex _mutex;

  std::map<uint32_t, uint32_t> _nodeMap;
  StringMap _linkMap;
};

#endif //CSTATUSWRITER_H

