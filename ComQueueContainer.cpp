#include "ComQueueContainer.h"

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
    case def::e_Logged:
    {
      return m_logged_cmd;
    }
    case def::e_Ping:
    {
      return m_ping_cmd;
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
    case def::e_Logged:
    {
      m_logged_cmd.push_back(p);
      break;
    }
    case def::e_Ping:
    {
      m_ping_cmd.push_back(p);
      break;
    }
    case def::e_CmdDefault:
    default:
    {
      m_default_cmd.push_back(p);
    }
  }
}


CmdQueueIter ComQueueContainer::begin()
{
  return m_default_cmd.begin();
} 

CmdQueueIter ComQueueContainer::end()
{
  return m_default_cmd.end();
}

CmdQueueIter ComQueueContainer::begin(def::t_CmdTypes cmdType)
{
  CmdQueueIter iter = m_default_cmd.begin();
  switch (cmdType)
  {
    case def::e_CmdDeallocMem:
    {
      iter = m_deallocPrecond_cmd.begin();
      break;
    }
    
    case def::e_CmdEndOfCycle:
    {
      iter = m_endOfCycle_cmd.begin();
      break;
    }

    case def::e_Logged:
    {
      iter = m_logged_cmd.begin();
      break;
    }
    
    case def::e_Ping:
    {
      iter = m_ping_cmd.begin();
      break;
    }
    
    default:
    {
      std::cerr << "ComQueueContainer::begin: undefined queue type!" << std::endl;
    }
  }

  return iter;
}

CmdQueueIter ComQueueContainer::end(def::t_CmdTypes cmdType)
{
  CmdQueueIter iter = m_default_cmd.end();
  switch (cmdType)
  {
    case def::e_CmdDeallocMem:
    {
      iter = m_deallocPrecond_cmd.end();
      break;
    }
    
    case def::e_CmdEndOfCycle:
    {
      iter = m_endOfCycle_cmd.end();
      break;
    }

    case def::e_Logged:
    {
      iter = m_logged_cmd.end();
      break;
    }
    
    case def::e_Ping:
    {
      iter = m_ping_cmd.end();
      break;
    }
  
    default:
    {
      std::cerr << "ComQueueContainer::end: undefined queue type!" << std::endl;
    }
  }

  return iter;
}