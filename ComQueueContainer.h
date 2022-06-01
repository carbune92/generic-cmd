#ifndef __COMQUEUECONTAINER_H__
#define __COMQUEUECONTAINER_H__

#include <vector>
#include <memory>
#include "Cmd.h"

typedef std::vector<def::GenCmdPtr_t>::iterator CmdQueueIter;

class ComQueueContainer
{
  public:
  std::vector<def::GenCmdPtr_t>& get_CmdQ(def::t_CmdTypes cmdType = def::e_CmdDefault);
  
  void push_back(def::GenCmdPtr_t p, def::t_CmdTypes cmdType = def::e_CmdDefault);


  CmdQueueIter begin();
  CmdQueueIter end();
  CmdQueueIter begin(def::t_CmdTypes cmdType);
  CmdQueueIter end(def::t_CmdTypes cmdType);

  private:
  std::vector<def::GenCmdPtr_t> m_default_cmd;
  std::vector<def::GenCmdPtr_t> m_endOfCycle_cmd;
  std::vector<def::GenCmdPtr_t> m_deallocPrecond_cmd;
  std::vector<def::GenCmdPtr_t> m_logged_cmd;
  std::vector<def::GenCmdPtr_t> m_ping_cmd;
};

#endif 