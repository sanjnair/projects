/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CKBHANDLER_H
#define CKBHANDLER_H

#include "cglobal.h"
#include "ciservant.h"
#include "cthread.h"
#include "cmutexlocker.h"

#define MAX_USER_INPUT 1000

class CKbHandler : public CThread
{
public:
  CKbHandler();
  ~CKbHandler();

  void run();
  void stop();
  void setServantHandle(IServant *s);
  void opInterrupted();
  void outputStr(const string &str, bool newLine=false);
  string getUserInput() const;

private:
  bool isStopFlagEnabled();
  void setOpInProgress(bool flag);
  bool isOpInProgress();
  void outputPrompt();
  void handleUserInput();
  bool processInput(const string &input);
  void initSigHandler();
  void errorIfInvalidSigCode(int code, const string &method) const;

  bool _stopFlag;
  bool _opInProgress;
  IServant *_servant;
  string _prompt;
  struct sigaction _sigAct;
  sigset_t _sigmask;
  CMutex _mutex;

};

#endif //CKBHANDLER_H
