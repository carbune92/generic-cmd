#ifndef __COMQUEUECONTAINER_H__
#define __COMQUEUECONTAINER_H__

#include <vector>
#include <memory>
#include <optional>
#include "Cmd.h"

class ComQueueContainer
{
  public:
  // std::vector<def::GenCmdPtr_t>& get_Default_CmdQ();

  std::vector<def::GenCmdPtr_t>& get_CmdQ(def::t_CmdTypes cmdType = def::e_CmdDefault);

  void push_back(def::GenCmdPtr_t p, def::t_CmdTypes cmdType = def::e_CmdDefault);

  private:
  std::vector<def::GenCmdPtr_t> m_default_cmd;
  std::vector<def::GenCmdPtr_t> m_endOfCycle_cmd;
  std::vector<def::GenCmdPtr_t> m_deallocPrecond_cmd;
};

#endif 