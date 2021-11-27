#include "ComQueueContainer.h"

// std::vector<def::GenCmdPtr_t>& ComQueueContainer::get_Default_CmdQ()
// {
//   return m_default_cmd;
// }

std::vector<def::GenCmdPtr_t>& ComQueueContainer::get_CmdQ(def::t_CmdTypes cmdType)
{ 
  switch(cmdType)
  {
    case def::e_CmdDeallocMem:
    {
      return m_deallocPrecond_cmd;      
    }
    case def::e_CmdEndOfCycle:
    {
      return m_endOfCycle_cmd;
    }
    case def::e_CmdDefault:
    default:
    {
      return m_default_cmd;
    }
  }

  return m_default_cmd;
} 

void ComQueueContainer::push_back(def::GenCmdPtr_t p, def::t_CmdTypes cmdType)
{
  switch(cmdType)
  {
    case def::e_CmdDeallocMem:
    {
      m_deallocPrecond_cmd.push_back(p);
      break;
    }
    case def::e_CmdEndOfCycle:
    {
      m_endOfCycle_cmd.push_back(p);
      break;
    }
    case def::e_CmdDefault:
    default:
    {
      m_default_cmd.push_back(p);
    }
  }
}