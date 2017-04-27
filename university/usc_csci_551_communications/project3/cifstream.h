/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CIFSTREAM_H
#define CIFSTREAM_H

#include "cabstractfstream.h"

class CIfStream : public CAbstractfStream {

public:
  CIfStream(CFile *f, bool binary);
  virtual ~CIfStream();

  virtual void open();
  virtual void close();
  void eof() const;
  string readLine();
  int read(char *buf, int maxLen);

private:
  bool _binary;
  int _curPos;
  int _fileSize ;
};

#endif //CIFSTREAM_H

