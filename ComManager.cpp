#include "ComManager.h"
#include <iostream>
#include <algorithm>

using namespace cmd_format;

ComManager::ComManager(ComServerContainer* servContainer, ComQueueContainer* commContainer) : 
  itsServContainer{servContainer},
  itsCommContainer{commContainer},
  m_byteStream{}
{}

ComManager::ComManager() {}

ComManager::~ComManager() 
{
  delete itsServContainer;
  delete itsCommContainer;
}

void ComManager::init(ComServerContainer* servContainer, ComQueueContainer* commContainer)
{
  itsServContainer = servContainer;
  itsCommContainer = commContainer;
}

ComServerContainer& ComManager::getServeContainer()
{
  return *itsServContainer;
}

ComQueueContainer& ComManager::getComQueueContainer()
{
  return *itsCommContainer;
}


void ComManager::getByteStream(unsigned char *bytes, int max_bytes)
{
  m_byteStream.clear();
  m_byteStream.insert(m_byteStream.begin(), bytes, bytes + max_bytes);

  // TODO trim the beginning of the byte stream to the start marker of the cmd info (if it's the case)
}

int ComManager::parseCommand()
{
  int res = 0;
  def::data_t::iterator beginCmdIt = m_byteStream.begin();
  def::data_t::iterator sepIt = std::find(m_byteStream.begin(), m_byteStream.end(), CMD_SEPARATOR);
  
  // for (auto b : m_byteStream)
  // {
  //   std::cout << std::hex << (int)b << " ";
  // }
  // std::cout << std::endl;
  // if (m_byteStream.end() == sepIt)
  // {
  //   res = -5;
  //   std::cerr << "ComManager::parseCommand: ";
  // }

  do
  {
    /* code */
    def::t_SID sid = *beginCmdIt;
    
    if (t_ServiceId::SERViCE_COMMAND_DISTRIBUTION == sid)
    {
      def::t_CID cid = *(beginCmdIt+1);

      switch(cid)
      {
        case t_CmdId::COMMAND_IMMEDIATE_REBOOT:
        {
          // res = parseRebootCmd(m_byteStream.begin()+2, m_byteStream.end());
          res = parseRebootCmd(beginCmdIt+2, sepIt);
          break;  
        }
        case t_CmdId::COMMAND_WATCHDOG:
        {
          // res = parseWatchdogCmd(m_byteStream.begin()+2, m_byteStream.end());
          res = parseWatchdogCmd(beginCmdIt+2, sepIt);
          break;
        }
        default:
        {
          std::cerr << "ComManager::parseCommand: unknown cid from SERViCE_COMMAND_DISTRIBUTION : " << cid << std::endl;
          res = -1;
        }
      }
    }
    else if (t_ServiceId::DIAGNOSTICS == sid)
    {
      def::t_CID cid = *(beginCmdIt+1);
      
      switch(cid)
      {
        case t_CmdId::COMMAND_PING:
        {
          res = parsePingCmd(beginCmdIt+2, sepIt, def::e_Ping);
          break;
        }
        default:
        {
          std::cerr << "ComManager::parseCommand: unknown cid from DIAGNOSTICS : " << cid << std::endl;
          res = -1;
        }
      }
    }
    else
    {
      res = -4;
      std::cerr << "ComManager::parseCommand: unknown sid = " << (int)sid << ", res = " << res << std::endl;
    }

    if (sepIt == m_byteStream.end())
    {
      break;
    }

    beginCmdIt = sepIt+1; 
    sepIt = std::find(beginCmdIt, m_byteStream.end(), CMD_SEPARATOR);

  } while (true);
  
  return res;
}

int ComManager::parseRebootCmd(def::data_t::iterator itBegin, def::data_t::iterator itEnd, def::t_CmdTypes cmdType)
{
  int res = 0;
  if (itBegin == itEnd)
  {
    std::cerr << "ComManager::parseRebootCmd: invalid iterators" << std::endl;
    res = -2;
  }
  else
  {
    int nr_params_bytes = t_restart::NR_BYTES_PARAMS_RESTART;
    def::data_t params{};
    params.insert(params.begin(), itBegin, itBegin+nr_params_bytes);
    
    auto p = std::make_shared<cmd::Cmd<Restart_Server>>(t_ServiceId::SERViCE_COMMAND_DISTRIBUTION, t_CmdId::COMMAND_IMMEDIATE_REBOOT);
    p->init(itsServContainer->get_Restart_server(), &Restart_Server::immediatePiRestart, params);


    // convert p to a pointer to a Cmd with policy 'Logged'
    auto p_logged = std::make_shared<cmd::Cmd<Restart_Server, policies::Logged>>(*p);
    itsCommContainer->push_back(p_logged, cmdType);
  }

  return res;
}


int ComManager::parseWatchdogCmd(def::data_t::iterator itBegin, def::data_t::iterator itEnd, def::t_CmdTypes cmdType)
{
  int res = 0;
  if (itBegin == itEnd)
  {
    std::cerr << "ComManager::parseWatchdogCmd: invalid iterators" << std::endl;
    res = -2;
  }
  else
  {
    def::data_t params{};
    auto p = std::make_shared<cmd::Cmd<PiWatcherServer>>(t_ServiceId::SERViCE_COMMAND_DISTRIBUTION, t_CmdId::COMMAND_WATCHDOG);

    uint8_t watchdog_reg = *itBegin;

    switch(watchdog_reg)
    {
      case t_watchdog::REG_WATCHDOG_WAKE:
      {
        int nr_params_bytes = t_watchdog::NR_BYTES_PARAMS_WAKE;
        params.insert(params.begin(), itBegin, itBegin+nr_params_bytes);
        p->init(itsServContainer->get_Piwatcher_server(), &PiWatcherServer::wake, params);
        break;
      }
      case t_watchdog::REG_WATCHDOG_WATCH:
      {
        int nr_params_bytes = t_watchdog::NR_BYTES_PARAMS_WATCH;
        params.insert(params.begin(), itBegin, itBegin+nr_params_bytes);
        p->init(itsServContainer->get_Piwatcher_server(), &PiWatcherServer::watch, params);
        break;
      }
      case t_watchdog::REG_WATCHDOG_DUMP:
      {
        int nr_params_bytes = t_watchdog::NR_BYTES_PARAMS_DUMP;
        params.insert(params.begin(), itBegin, itBegin+nr_params_bytes);
        p->init(itsServContainer->get_Piwatcher_server(), &PiWatcherServer::dump, params);
      }
      default:  
      {
        res = -3;
        std::cerr << "ComManager::parseWatchdogCmd: invalid watchdog command" << std::endl;
      }
    }

    itsCommContainer->push_back(p, cmdType);
  }
  
  return res;
}

int ComManager::parsePingCmd(def::data_t::iterator itBegin, def::data_t::iterator itEnd, def::t_CmdTypes cmdType)
{
  int res = 0;
  
  if (itBegin == itEnd)
  {
    std::cerr << "ComManager::parsePingCmd invalid iterators" << std::endl;
    res = -2;
  }
  else
  {
    def::data_t timedate_param{};
    def::data_t ack_param{};
    auto p = std::make_shared<cmd::Cmd<PingServer>>(t_ServiceId::DIAGNOSTICS, t_CmdId::COMMAND_PING);
    
    timedate_param.insert(timedate_param.begin(), itBegin, itBegin+t_ping::NR_BYTES_TD);
    ack_param.insert(ack_param.begin(), itBegin+t_ping::NR_BYTES_TD, itBegin+t_ping::NR_BYTES_TD+t_ping::NR_BYTES_ACK);
    
    def::data_t params{timedate_param};
    params.insert(params.end(), ack_param.begin(), ack_param.end());
    
    p->init(itsServContainer->get_Ping_server(), &PingServer::addToModemQueue, params);
    
    itsCommContainer->push_back(utilfunc::addPolicy<PingServer,policies::Logged>(p), cmdType);
  }
  
  return res;
}

