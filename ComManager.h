#ifndef __COMMANAGER_H__
#define __COMMANAGER_H__

#include "ComServerContainer.h"
#include "ComQueueContainer.h"

class ComManager
{
  public:
  explicit ComManager(ComServerContainer& servContainer, ComQueueContainer& commContainer); 
  int parseCommand();
  void getByteStream(unsigned char *bytes, int max_bytes);

  private:
  int parseRebootCmd(def::data_t::iterator itBegin, def::data_t::iterator itEnd, def::t_CmdTypes cmdType = def::e_CmdDefault);
  int parseWatchdogCmd(def::data_t::iterator itBegin, def::data_t::iterator itEnd, def::t_CmdTypes cmdType = def::e_CmdDefault);
  ComServerContainer& itsServContainer;
  ComQueueContainer& itsCommContainer;
  def::data_t m_byteStream;
};

#endif