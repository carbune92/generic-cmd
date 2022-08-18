#include "PiWatcher_Server.h"
#include "Restart_Server.h"
#include "Ping_Server.h"
// #include "Cmd.h"

#include "ComServerContainer.h"
#include "ComManager.h"
#include "ComQueueContainer.h"

#include "Comm_RFD868.h"

#include <vector>
#include <memory> 
#include <iostream>
#include <cassert>

#define PASSED_M(msg) (std::cout << "\033[1;32mPASSED\033[0m : " msg "\n") 

Comm_RFD868 modem{};

ComManager init()
{
  std::shared_ptr<PiWatcherServer> p_piWatchserver = std::make_shared<PiWatcherServer>();
  std::shared_ptr<Restart_Server> p_restartServer = std::make_shared<Restart_Server>();
  p_piWatchserver->setup();
  p_restartServer->setup();
  
  std::shared_ptr<PingServer> p_pingServer = std::make_shared<PingServer>(modem);
  p_pingServer->setup();

  ComQueueContainer* qContainer = new ComQueueContainer;
  ComServerContainer* sContainer = new ComServerContainer{p_piWatchserver, p_restartServer, p_pingServer};
  ComManager cmdManager{sContainer, qContainer};
  
  return cmdManager;
}

void variousTests()
{
  std::cout << "START_TEST : " << __FUNCTION__ << std::endl;
  std::shared_ptr<PiWatcherServer> p_server = std::make_shared<PiWatcherServer>();
  p_server->setup();

  cmd::Cmd<PiWatcherServer> piwatch_cmd{2,4};
  piwatch_cmd.init(p_server, &PiWatcherServer::watch, {1,100});
  piwatch_cmd.execute();

  auto p_restart_server = std::make_shared<Restart_Server>();
  cmd::Cmd<Restart_Server> restart_cmd{2,1};
  restart_cmd.init(p_restart_server, &Restart_Server::immediatePiRestart, {});
  restart_cmd.execute();

  cmd::Generic_Cmd *p_gen[2];
  p_gen[0] = &piwatch_cmd;
  p_gen[1] = &restart_cmd;

  p_gen[0]->execute();
  p_gen[1]->execute();
  
  std::shared_ptr<PingServer> p_pingServer = std::make_shared<PingServer>(modem);

  ComQueueContainer* qContainer = new ComQueueContainer;
  ComServerContainer* sContainer = new ComServerContainer{p_server, p_restart_server, p_pingServer};

  ComManager cmdManager{sContainer, qContainer};

  auto p = std::make_shared<cmd::Cmd<Restart_Server>>(2,1);
  qContainer->push_back(p);
  // qContainer->get_CmdQ().push_back(p);

  cmd::Cmd<Restart_Server> smen{1,1};
  std::cout << (int)smen.getC_ID() << '\n';
  smen = cmd::Cmd<Restart_Server>{1,2};
  std::cout << (int)smen.getC_ID() << '\n';


  qContainer->get_CmdQ().clear();
  unsigned char buffer[256] = {{0}};
  buffer[0] = cmd_format::t_ServiceId::SERViCE_COMMAND_DISTRIBUTION;
  buffer[1] = cmd_format::t_CmdId::COMMAND_IMMEDIATE_REBOOT;
  cmdManager.getByteStream(&buffer[0], sizeof(buffer));
  int res = cmdManager.parseCommand();
  std::cout << "Res parse cmd reboot: " << res << std::endl;
  assert(res >= 0);
  PASSED_M("Cmd reboot parsed.");
  
  std::cout << qContainer->get_CmdQ().size() << '\n';
  def::GenCmdPtr_t gen_cmd_ptr = qContainer->get_CmdQ().front();
  gen_cmd_ptr->execute();
  gen_cmd_ptr = nullptr;

  buffer[0] = cmd_format::t_ServiceId::SERViCE_COMMAND_DISTRIBUTION;
  buffer[1] = cmd_format::t_CmdId::COMMAND_WATCHDOG;
  buffer[2] = cmd_format::t_watchdog::REG_WATCHDOG_WATCH;
  buffer[3] = 0;
  buffer[4] = static_cast<unsigned char>(60);
  cmdManager.getByteStream(&buffer[0], sizeof(buffer));
  res = cmdManager.parseCommand();
  std::cout << "Res parse cmd watchdog watch: " << res << std::endl;
  assert(res >= 0);
  PASSED_M("Cmd watchdog watch parsed.");
  
  std::cout << qContainer->get_CmdQ().size() << '\n';
  gen_cmd_ptr = qContainer->get_CmdQ().back();
  gen_cmd_ptr->execute();

  std::fill(buffer, buffer+sizeof(buffer), 0U);
  qContainer->get_CmdQ().clear();
  buffer[0] = cmd_format::t_ServiceId::SERViCE_COMMAND_DISTRIBUTION;
  buffer[1] = cmd_format::t_CmdId::COMMAND_WATCHDOG;
  buffer[2] = cmd_format::t_watchdog::REG_WATCHDOG_WATCH;
  buffer[3] = 0;
  buffer[4] = static_cast<unsigned char>(60);
  // buffer[3] = cmd_format::CMD_SEPARATOR;
  buffer[5] = cmd_format::t_ServiceId::SERViCE_COMMAND_DISTRIBUTION;
  buffer[6] = cmd_format::t_CmdId::COMMAND_IMMEDIATE_REBOOT;

  // std::cout << '\n';
  // for (int i=0; i<7; i++)
  // {
  //   std::cout << std::hex << (int)buffer[i] << " ";
  // }
  // std::cout << '\n';

  cmdManager.getByteStream(&buffer[0], sizeof(buffer));
  res = cmdManager.parseCommand();  
  std::cout << "Res parse cmd watchdog watch and immediate reboot: " << res << std::endl;
  assert(res >= 0);
  PASSED_M("Cmd watchdog watch and immediate reboot parsed.");
  
  std::cout << qContainer->get_CmdQ().size() << '\n';

  for (auto p : qContainer->get_CmdQ())
  {
    p->execute();
  }

  auto startIt = qContainer->begin();
  auto endIt = qContainer->end();
  for (auto it = startIt; it != endIt; it++)
  {
    (*it)->execute();
  }

   // foreach works with overloaded no-parameters begin/end 
  for (auto p : *qContainer)
  {
    p->execute();
  }

  // cmd::Cmd<Restart_Server, policies::ExecAtEndOfCycle> polCmd{2,1};
  auto p_polCmd = std::make_shared<cmd::Cmd<Restart_Server, policies::ExecAtEndOfCycle>>(2,1);
  p_polCmd->init(p_restart_server, &Restart_Server::immediatePiRestart, {});
  p_polCmd->execute();

  qContainer->get_CmdQ().clear();
  qContainer->push_back(p_polCmd, def::t_CmdTypes::e_CmdEndOfCycle);


  auto piwatcher_cmd = std::make_shared<cmd::Cmd<PiWatcherServer>>(2,4);

  cmd::Cmd<PiWatcherServer,policies::DeallocMemPrecond> piwatcher_cmd_pol2{2,4};
  
  cmd::Cmd<PiWatcherServer> piwatcher_cmd2{2,4};
  piwatcher_cmd2.init(p_server, &PiWatcherServer::status, {2});
  piwatcher_cmd2.execute();
  cmd::Cmd<PiWatcherServer,policies::DeallocMemPrecond> piwatcher_cmd_pol(piwatcher_cmd2);
  piwatcher_cmd_pol.dealloc();
  piwatcher_cmd_pol.execute();
  std::cout << (int)piwatcher_cmd2.getC_ID() << '\n';
  std::cout << (int)piwatcher_cmd_pol.getC_ID() << '\n';
  std::cout << (int)piwatcher_cmd_pol2.getC_ID() << '\n';


  piwatcher_cmd_pol2 = piwatcher_cmd_pol;
  piwatcher_cmd_pol2.dealloc();
  piwatcher_cmd_pol2.execute();

  std::cout << "END_TEST : " << __FUNCTION__ << std::endl;
}

void test_Logged_policy()
{
  std::cout << "START_TEST : " << __FUNCTION__ << std::endl;
  std::shared_ptr<PiWatcherServer> p_piWatchserver = std::make_shared<PiWatcherServer>();
  std::shared_ptr<Restart_Server> p_restartServer = std::make_shared<Restart_Server>();
  p_piWatchserver->setup();
  p_restartServer->setup();
  
  std::shared_ptr<PingServer> p_pingServer = std::make_shared<PingServer>(modem);

  ComQueueContainer* qContainer = new ComQueueContainer;
  ComServerContainer* sContainer = new ComServerContainer{p_piWatchserver, p_restartServer, p_pingServer};
  ComManager cmdManager{sContainer, qContainer};

  qContainer->get_CmdQ().clear();
  unsigned char buffer[256] = {{0}};
  buffer[0] = cmd_format::t_ServiceId::SERViCE_COMMAND_DISTRIBUTION;
  buffer[1] = cmd_format::t_CmdId::COMMAND_IMMEDIATE_REBOOT;
  cmdManager.getByteStream(&buffer[0], sizeof(buffer));
  int res = cmdManager.parseCommand();
  std::cout << "Res parse cmd reboot: " << res << std::endl;
  assert(res >= 0);
  PASSED_M("Cmd reboot parsed.");

  // cmd::Cmd<Restart_Server,policies::Logged> restart;
  std::shared_ptr<cmd::Cmd<Restart_Server,policies::Logged>> restart;
  def::GenCmdPtr_t p_cp;

  for (auto p : *qContainer)
  {
    bool isinst = utilfunc::isInstanceOf<cmd::Cmd<Restart_Server, policies::Logged>>(p);
    assert(isinst);
    PASSED_M("p is instance of Cmd<Restart_Server, policies::Logged>.");
    isinst = utilfunc::isInstanceOf<policies::Logged>(p);
    assert(isinst);
    PASSED_M("p is instance of policies::Logged.");

    if (utilfunc::isInstanceOf<cmd::Cmd<Restart_Server, policies::Logged>>(p))
    {
      // restart = dynamic_cast<cmd::Cmd<Restart_Server,policies::Logged>&>(*p);
      auto& temp = dynamic_cast< cmd::Cmd<Restart_Server, policies::Logged>& >(*p);
      restart = std::make_shared<cmd::Cmd<Restart_Server, policies::Logged>>(temp);
      std::cout << "YES, this is a valid instance of Logged!\n";
      policies::Logged& l = dynamic_cast<policies::Logged&>(*p);
      l.setFileStream("./restart_log.txt");
      l.log("pi reboot will commence");
      p->execute();
    }
    else
    {
      std::cout << "NO, this is not a valid instance of Logged!\n";
    }

    p_cp = p;
    p = nullptr;
  }

  std::cout << "{" << (int)restart->getS_ID() << " " << (int)restart->getC_ID() << "}\n";
  restart->execute();

  // add new policy to restart
  auto p_logged_endcycle_cmd = 
    std::make_shared<cmd::Cmd<Restart_Server, policies::Logged, policies::ExecAtEndOfCycle>>(restart->getS_ID(), restart->getC_ID());
  p_logged_endcycle_cmd->init(restart->get_sptr_server(), restart->get_callback(), restart->get_req());
  p_logged_endcycle_cmd->execute();

  bool isinst = utilfunc::isInstanceOf<policies::ExecAtEndOfCycle>(p_logged_endcycle_cmd);
  assert(isinst);
  PASSED_M("p_logged_endcycle_cmd is an instance of ExecAtEndOfCycle.");
  
  if (utilfunc::isInstanceOf<policies::ExecAtEndOfCycle>(p_logged_endcycle_cmd))
  {
    std::cout << "YES, this is a valid instance of ExecAtEndOfCycle!\n";
  }
  else
  {
    std::cout << "NO, this is not a valid instance of ExecAtEndOfCycle!\n";
  }

  // auto p_logged_dealloc_cmd = utilfunc::addPolicy< cmd::Cmd<Restart_Server, policies::Logged, policies::DeallocMemPrecond>, 
  //                                                  cmd::Cmd<Restart_Server, policies::Logged> >(p_cp);

  auto p_logged_dealloc_cmd = utilfunc::addPolicy< Restart_Server, policies::DeallocMemPrecond, policies::Logged, policies::ExecAtEndOfCycle >(p_logged_endcycle_cmd);
  p_logged_dealloc_cmd->execute();

  isinst = utilfunc::isInstanceOf<policies::DeallocMemPrecond>(p_logged_dealloc_cmd);
  assert(isinst);
  PASSED_M("p_logged_endcycle_cmd is an instance of DeallocMemPrecond.");
  
  if (utilfunc::isInstanceOf<policies::DeallocMemPrecond>(p_logged_dealloc_cmd))
  {
    std::cout << "YES, this is a valid instance of DeallocMemPrecond!\n";
  }
  else
  {
    std::cout << "NO, this is not a valid instance of DeallocMemPrecond!\n";
  }

  isinst = utilfunc::isInstanceOf<policies::ExecAtEndOfCycle>(p_logged_dealloc_cmd);
  assert(isinst);
  PASSED_M("p_logged_endcycle_cmd is an instance of ExecAtEndOfCycle.");
  
  if (utilfunc::isInstanceOf<policies::ExecAtEndOfCycle>(p_logged_dealloc_cmd))
  {
    std::cout << "YES, this is a valid instance of ExecAtEndOfCycle!\n";
  }
  else
  {
    std::cout << "NO, this is not a valid instance of ExecAtEndOfCycle!\n";
  } 

  std::cout << "{" << (int)p_logged_endcycle_cmd->getS_ID() << " " << (int)p_logged_endcycle_cmd->getC_ID() << "}\n";
  policies::DeallocMemPrecond& deall = *p_logged_dealloc_cmd;
  std::cout << "{" << (int)deall.getS_ID() << " " << (int)deall.getC_ID() << "}\n";
  std::cout << "{" << (int)p_logged_dealloc_cmd->getS_ID() << " " << (int)p_logged_dealloc_cmd->getC_ID() << "}\n";
  
  deall.execute();
  deall.dealloc();

  std::cout << "END_TEST : " << __FUNCTION__ << std::endl;
}

void test_isInstanceOf_negative()
{
  std::cout << "START_TEST : " << __FUNCTION__ << std::endl;
  std::shared_ptr<PiWatcherServer> p_piWatchserver = std::make_shared<PiWatcherServer>();
  std::shared_ptr<Restart_Server> p_restartServer = std::make_shared<Restart_Server>();
  p_piWatchserver->setup();
  p_restartServer->setup();
  
  std::shared_ptr<PingServer> p_pingServer = std::make_shared<PingServer>(modem);

  ComQueueContainer* qContainer = new ComQueueContainer;
  ComServerContainer* sContainer = new ComServerContainer{p_piWatchserver, p_restartServer, p_pingServer};
  ComManager cmdManager{sContainer, qContainer};

  unsigned char buffer[256];
  std::fill(buffer, buffer+sizeof(buffer), 0U);
  qContainer->get_CmdQ().clear();
  buffer[0] = cmd_format::t_ServiceId::SERViCE_COMMAND_DISTRIBUTION;
  buffer[1] = cmd_format::t_CmdId::COMMAND_WATCHDOG;
  buffer[2] = cmd_format::t_watchdog::REG_WATCHDOG_WATCH;
  buffer[3] = static_cast<unsigned char>(60);

  cmdManager.getByteStream(&buffer[0], sizeof(buffer));
  int res = cmdManager.parseCommand();
  std::cout << "Res of parseCommand cmd watchdog watch " << res << std::endl;
  assert(res >= 0);
  PASSED_M("Cmd watchdog watch parsed.");
  
  for (auto p : *qContainer)
  {
    
    bool isinst = utilfunc::isInstanceOf<policies::Logged>(p);
    assert(!isinst);
    PASSED_M("p is NOT an instance of Logged.");
    if (utilfunc::isInstanceOf<policies::Logged>(p))
    {
      std::cout << "YES, this is a valid instance of Logged!\n";
    }
    else
    {
      std::cout << "NO, this is not a valid instance of Logged!\n";
    }
    
    std::cout << "{" << (int)p->getS_ID() << " " << (int)p->getC_ID() << "}\n"; 
  }
  
  std::cout << "END_TEST : " << __FUNCTION__ << std::endl;
}

void test_copy()
{
  std::cout << "START_TEST : " << __FUNCTION__ << std::endl;
  ComManager cmdManager = init();
  ComQueueContainer& qContainer = cmdManager.getComQueueContainer();
  ComServerContainer& sContainer = cmdManager.getServeContainer();
  cmd::Cmd<Restart_Server,policies::Logged> cmd_restart_logged{2,1};
  
  {
    cmd::Cmd<Restart_Server,policies::Logged> cmd_restart_logged2{2,1};
    cmd_restart_logged2.init(sContainer.get_Restart_server(), &Restart_Server::immediatePiRestart, {});
    cmd_restart_logged2.execute();
    std::cout << "{" << (int)cmd_restart_logged2.getS_ID() << " " << (int)cmd_restart_logged2.getC_ID() << "}\n";
    assert(2 == (int)cmd_restart_logged2.getS_ID());
    assert(1 == (int)cmd_restart_logged2.getC_ID());
    PASSED_M("cmd_restart_logged2 has id {2,1}");

    cmd_restart_logged = cmd_restart_logged2;
  }
  cmd_restart_logged.execute();
  std::cout << "{" << (int)cmd_restart_logged.getS_ID() << " " << (int)cmd_restart_logged.getC_ID() << "}\n"; 
  assert(2 == (int)cmd_restart_logged.getS_ID());
  assert(1 == (int)cmd_restart_logged.getC_ID());
  PASSED_M("cmd_restart_logged has id {2,1}");
  
  cmd::Cmd<Restart_Server,policies::Logged> cmd_restart_logged3{cmd_restart_logged};
  cmd_restart_logged3.execute();
  std::cout << "{" << (int)cmd_restart_logged3.getS_ID() << " " << (int)cmd_restart_logged3.getC_ID() << "}\n"; 
  assert(2 == (int)cmd_restart_logged3.getS_ID());
  assert(1 == (int)cmd_restart_logged3.getC_ID());
  PASSED_M("cmd_restart_logged3 has id {2,1}");
 
  cmd::Cmd<Restart_Server> cmd_restart{0,5};
  cmd::Cmd<Restart_Server> cmd_restart2{2,1};
  cmd_restart2.init(sContainer.get_Restart_server(), &Restart_Server::immediatePiRestart, {});
  assert(0 == (int)cmd_restart.getS_ID());
  assert(5 == (int)cmd_restart.getC_ID());
  PASSED_M("cmd_restart has id {0,5}");

  cmd_restart = cmd_restart2;
  std::cout << "{" << (int)cmd_restart.getS_ID() << " " << (int)cmd_restart.getC_ID() << "}\n"; 
  cmd_restart.execute();
  assert(2 == (int)cmd_restart.getS_ID());
  assert(1 == (int)cmd_restart.getC_ID());
  PASSED_M("cmd_restart has id {2,1} after assigment");

  cmd::Cmd<Restart_Server> cmd_restart3(cmd_restart);
  std::cout << "{" << (int)cmd_restart3.getS_ID() << " " << (int)cmd_restart3.getC_ID() << "}\n"; 
  cmd_restart3.execute();
  assert(2 == (int)cmd_restart3.getS_ID());
  assert(1 == (int)cmd_restart3.getC_ID());
  PASSED_M("cmd_restart3 has id {2,1}");

  cmd::Cmd<Restart_Server,policies::Logged> cmd_restart4{0,5};
  cmd_restart4 = cmd_restart3;
  std::cout << "{" << (int)cmd_restart4.getS_ID() << " " << (int)cmd_restart4.getC_ID() << "}\n"; 
  cmd_restart4.execute();
  assert(2 == (int)cmd_restart4.getS_ID());
  assert(1 == (int)cmd_restart4.getC_ID());
  PASSED_M("cmd_restart4 has id {2,1} after assignment");
  
  bool isinst = utilfunc::isInstanceOf<policies::Logged>(cmd_restart4);
  assert(isinst);
  PASSED_M("cmd_restart4 is instance of Logged.");
  
  if (utilfunc::isInstanceOf<policies::Logged>(cmd_restart4)) 
  {
    std::cout << "YES\n";
    policies::Logged& l = cmd_restart4;
    l.setFileStream("restart_log.txt");
  }

  cmd::Cmd<Restart_Server,policies::Logged> cmd_restart5(cmd_restart3);
  std::cout << "{" << (int)cmd_restart5.getS_ID() << " " << (int)cmd_restart5.getC_ID() << "}\n"; 
  cmd_restart5.execute();
  
  assert(2 == (int)cmd_restart5.getS_ID());
  assert(1 == (int)cmd_restart5.getC_ID());
  PASSED_M("cmd_restart5 has id {2,1}");
  
  isinst = utilfunc::isInstanceOf<policies::Logged>(cmd_restart5);
  assert(isinst);
  PASSED_M("cmd_restart5 is instance of Logged.");
  if (utilfunc::isInstanceOf<policies::Logged>(cmd_restart5)) 
  {
    std::cout << "YES\n";
    policies::Logged& l = cmd_restart5;
    l.setFileStream("restart_log.txt");
    l.execute();
  }

  cmd::Generic_Cmd& gen  = cmd_restart5;
  cmd::Cmd<Restart_Server,policies::Logged> cmd_restart6;
  cmd_restart6 = gen;
  std::cout << "{" << (int)cmd_restart6.getS_ID() << " " << (int)cmd_restart6.getC_ID() << "}\n";
  assert(2 == (int)cmd_restart6.getS_ID());
  assert(1 == (int)cmd_restart6.getC_ID());
  PASSED_M("cmd_restart6 has id {2,1}");
  assert(2 == (int)gen.getS_ID());
  assert(1 == (int)gen.getC_ID());
  PASSED_M("gen has id {2,1}");
  
  try
  {
    // this will generat std::bad_function_call!! (as expected)
    cmd_restart6.execute();  
  }
  catch (std::bad_function_call& e)
  {
    assert(true);
    PASSED_M("cmd_restart6.execute() cannot execute callback because copy assigment between cmd::Cmd and Generic_Cmd!");
  }
  
  try
  {
    // this will generat std::bad_function_call!! (as expected)
    gen.execute();  
  }
  catch (std::bad_function_call& e)
  {
    assert(false);
  }
  PASSED_M("gen.execute() WILL execute callback because it's reference to an initialized cmd.");
  

  cmd::Cmd<Restart_Server,policies::Logged> cmd_restart7(gen);
  std::cout << "{" << (int)cmd_restart7.getS_ID() << " " << (int)cmd_restart7.getC_ID() << "}\n";
  
  assert(2 == (int)cmd_restart7.getS_ID());
  assert(1 == (int)cmd_restart7.getC_ID());
  PASSED_M("cmd_restart7 has id {2,1}");
  assert(2 == (int)gen.getS_ID());
  assert(1 == (int)gen.getC_ID());
  PASSED_M("gen has id {2,1}");
 
  try
  {
    // this will generat std::bad_function_call!! (as expected)
    cmd_restart7.execute();  
  }
  catch (std::bad_function_call& e)
  {
    assert(true);
    PASSED_M("cmd_restart7.execute() cannot execute callback because copy constructor between cmd::Cmd and Generic_Cmd!");
  }
  
  // this will generat std::bad_function_call!! (as expected)
  // cmd_restart7.execute();

  std::cout << "END_TEST : " << __FUNCTION__ << std::endl;
}

void test_ping()
{
  std::cout << "START_TEST : " << __FUNCTION__ << std::endl;
  ComManager cmdManager = init();
  ComQueueContainer& qContainer = cmdManager.getComQueueContainer();
  ComServerContainer& sContainer = cmdManager.getServeContainer();
  
  // cmd::Cmd<PingServer> cmd_ping{3,1};
  auto cmd_ping = std::make_shared<cmd::Cmd<PingServer>>(3,1);
  std::cout << "cmd_ping SID " << (int)cmd_ping->getS_ID() << " and CID " << (int)cmd_ping->getC_ID() << '\n';
  
  // for datetime: 21-05-2022 22-05-59|
  
  std::vector<uint8_t> a_param  = {'2','1','0','5','2','0','2','2','2','2','0','5','5','9'};
  int ack = 250;
  // a_param.push_back(static_cast<uint8_t>((ack >> 24) & 0xff));
  // a_param.push_back(static_cast<uint8_t>((ack >> 16) & 0xff));
  a_param.push_back(static_cast<uint8_t>((ack >> 8) & 0xff));
  a_param.push_back(static_cast<uint8_t>(ack & 0xff));
  
  cmd_ping->init(sContainer.get_Ping_server(), &PingServer::addToModemQueue, a_param);
  cmd_ping->execute();

  std::ostringstream ss;
  modem.getPingQueue().clear();
  for (auto p : modem.getPingQueue())
  {
    std::cout << p.recv_tm << " : " << p.recv_ack << std::endl;
    ss << p.recv_tm << " : " << p.recv_ack << std::endl;
  }
  
  auto p_logged_ping_cmd = utilfunc::addPolicy<PingServer, policies::Logged>(cmd_ping);
  
  if (p_logged_ping_cmd)
  {
    const char pinglog[] = "./ping_log.txt";
    policies::Logged& l = *p_logged_ping_cmd;
    l.setFileStream(pinglog);
    l.log(ss.str());
  }
  
  p_logged_ping_cmd->execute();
  for (auto p : modem.getPingQueue())
  {
    std::cout << p.recv_tm << " : " << p.recv_ack << std::endl;
  }
  
  auto p_copy = p_logged_ping_cmd;
  p_copy->execute();
  
  for (auto p : modem.getPingQueue())
  {
    std::cout << p.recv_tm << " : " << p.recv_ack << std::endl;
  }
  
  auto pingServer = sContainer.get_Ping_server();
  std::cout << pingServer->req2str( p_copy->get_req() ) << std::endl;
  
  std::shared_ptr<policies::Logged> p_l2 = p_copy;
  p_l2->setFileStream("./ping_log.txt");
  
  std::string pingstr= pingServer->req2str(p_copy->get_req());
  p_l2->log(pingstr);
}

void test_cmptm()
{
  std::string tm1{"18-12-1992 03-45-30"};
  std::string tm2{"18-12-1992 01-45-30"};
  std::cout << cmptm(tm1,tm2) << std::endl;
  
  tm1 = "18-12-1992 03-45-30";
  tm2 = "18-12-1992 05-45-30";
  std::cout << cmptm(tm1,tm2) << std::endl;
  
  tm1 = "11-05-1999 03-45-30";
  tm2 = "18-12-1992 03-45-30";
  std::cout << cmptm(tm1,tm2) << std::endl;
  
  tm1 = "11-05-1999 03-45-30";
  tm2 = "18-12-2022 23-11-30";
  std::cout << cmptm(tm1,tm2) << std::endl;
}

void test_decode_ping()
{
  std::cout << "START_TEST : " << __FUNCTION__ << std::endl;
  ComManager cmdManager = init();
  ComQueueContainer& qContainer = cmdManager.getComQueueContainer();
  ComServerContainer& sContainer = cmdManager.getServeContainer();
  
  std::shared_ptr<PingServer> p_pingS = sContainer.get_Ping_server();
  
  std::vector<uint8_t> a_param{cmd_format::t_ServiceId::DIAGNOSTICS, cmd_format::t_CmdId::COMMAND_PING};
  a_param.insert(a_param.end(), {'0','1','0','6','2','0','2','2','1','2','1','3','0','5'});
  int ack = 923;
  // a_param.push_back(static_cast<uint8_t>((ack >> 24) & 0xff));
  // a_param.push_back(static_cast<uint8_t>((ack >> 16) & 0xff));
  a_param.push_back(static_cast<uint8_t>((ack >> 8) & 0xff));
  a_param.push_back(static_cast<uint8_t>(ack & 0xff));
  
  cmdManager.getByteStream(&a_param[0], a_param.size());
  int res = cmdManager.parseCommand();
  std::cout << "Res of parsed ping cmd: " << res << std::endl;
  assert(res >= 0);
  PASSED_M("Ping cmd was parsed.");
  
  modem.getPingQueue().clear();
  assert(1 == qContainer.get_CmdQ(def::e_Ping).size());
  PASSED_M("Ping Q has 1 element.");
  
  if (qContainer.get_CmdQ(def::e_Ping).size() > 0)
  {
    for (auto p : qContainer.get_CmdQ(def::e_Ping))
    {
      p->execute();
      
      bool isinst = utilfunc::isInstanceOf<cmd::Cmd<PingServer,policies::Logged>>(p);
      assert(isinst);
      PASSED_M("p is an instance of cmd::Cmd<PingServer,policies::Logged>.");
      
      if (/*utilfunc::hasServerType(cmd_format::t_ServiceId::DIAGNOSTICS, cmd_format::t_CmdId::COMMAND_PING, p) && */
          utilfunc::isInstanceOf<cmd::Cmd<PingServer,policies::Logged>>(p))
      {
        policies::Logged& l = dynamic_cast<policies::Logged&>(*p);
        // auto& ping = dynamic_cast<cmd::Cmd<PingServer,policies::Logged>&>(*p);
        l.setFileStream("./ping_log.txt");
        
        std::string logStr = p_pingS->req2str(p->get_req());

        // std::string logStr = sContainer.get_Ping_server()->req2str(ping.get_req());
        l.log(logStr);
        
        // do it also via double dispatch
        l.log(sContainer.get_Ping_server());
      }
      else
      {
        std::cerr << "not of Logged type or does not have PingServer!\n";
      }
      
      if (modem.getPingQueue().size() == 0)
      {
        std::cerr << "Modem queue is empty!\n";
        break;
      }
      
      assert(1 == modem.getPingQueue().size());
      PASSED_M("Modem Q has 1 element.");
      
      for (auto q : modem.getPingQueue())
      {
        std::cout << q.recv_tm << " : " << q.recv_ack << std::endl;
      }
    }
  }
  else
  {
    std::cerr << "Ping queue empty!\n";
  }
}

void test_decode_ping_wrong_td()
{
  std::cout << "START_TEST : " << __FUNCTION__ << std::endl;
  ComManager cmdManager = init();
  ComQueueContainer& qContainer = cmdManager.getComQueueContainer();
  ComServerContainer& sContainer = cmdManager.getServeContainer();
  
  std::shared_ptr<PingServer> p_pingS = sContainer.get_Ping_server();
  
  std::vector<uint8_t> a_param{cmd_format::t_ServiceId::DIAGNOSTICS, cmd_format::t_CmdId::COMMAND_PING};
  
  // insert a future timedate than the current time: 20 June 2125...
  a_param.insert(a_param.end(), {'2','0','0','6','2','1','2','5','1','2','1','3','0','5'});
  int ack = 400;
  // a_param.push_back(static_cast<uint8_t>((ack >> 24) & 0xff));
  // a_param.push_back(static_cast<uint8_t>((ack >> 16) & 0xff));
  a_param.push_back(static_cast<uint8_t>((ack >> 8) & 0xff));
  a_param.push_back(static_cast<uint8_t>(ack & 0xff));
  
  qContainer.get_CmdQ(def::e_Ping).clear();
  modem.getPingQueue().clear();
  cmdManager.getByteStream(&a_param[0], a_param.size());
  cmdManager.parseCommand();
  
  assert(qContainer.get_CmdQ(def::e_Ping).size() == 1);
  
  std::vector<def::GenCmdPtr_t>::iterator it = qContainer.get_CmdQ(def::e_Ping).begin();
  (*it)->execute();
  
  assert(modem.getPingQueue().size() == 0);
  std::cout << "Cmd queue is empty because the timedate in the stream is a future one!\n";
}

void test_decode_ping_new_queue()
{
    std::cout << "START_TEST : " << __FUNCTION__ << std::endl;
  ComManager cmdManager = init();
  ComQueueContainer& qContainer = cmdManager.getComQueueContainer();
  ComServerContainer& sContainer = cmdManager.getServeContainer();
  
  std::shared_ptr<PingServer> p_pingS = sContainer.get_Ping_server();
  
  std::vector<uint8_t> a_param{cmd_format::t_ServiceId::DIAGNOSTICS, cmd_format::t_CmdId::COMMAND_PING};
  
  a_param.insert(a_param.end(), {'2','9','0','6','2','0','2','2','1','5','2','3','5','5'});
  int ack = 695;
  a_param.push_back(static_cast<uint8_t>((ack >> 8) & 0xff));
  a_param.push_back(static_cast<uint8_t>(ack & 0xff));
  
  qContainer.get_CmdQ(def::e_Ping).clear();
  modem.getPingQueue().clear();
  modem.getPingQueuePtr().clear();
  
  cmdManager.getByteStream(&a_param[0], a_param.size());
  int res = cmdManager.parseCommand();
  std::cout << "Res of parsed ping cmd: " << res << std::endl;
  assert(res >= 0);
  PASSED_M("Ping cmd was parsed.");
  
  assert(1 == qContainer.get_CmdQ(def::e_Ping).size());
  PASSED_M("Ping Q has 1 element.");
  
  for (auto p : qContainer.get_CmdQ(def::e_Ping))
  {
    p->execute();
      
    bool isinst = utilfunc::isInstanceOf<cmd::Cmd<PingServer,policies::Logged>>(p);
    assert(isinst);
    PASSED_M("p is an instance of cmd::Cmd<PingServer,policies::Logged>.");
    
    if (/*utilfunc::hasServerType(cmd_format::t_ServiceId::DIAGNOSTICS, cmd_format::t_CmdId::COMMAND_PING, p) && */
        utilfunc::isInstanceOf<cmd::Cmd<PingServer,policies::Logged>>(p))
    {
      policies::Logged& l = dynamic_cast<policies::Logged&>(*p);
      // auto& ping = dynamic_cast<cmd::Cmd<PingServer,policies::Logged>&>(*p);
      l.setFileStream("./ping_log.txt");
      
      std::string logStr = p_pingS->req2str(p->get_req());

      // std::string logStr = sContainer.get_Ping_server()->req2str(ping.get_req());
      l.log(logStr);
      
      // do it also via double dispatch
      l.log(sContainer.get_Ping_server());
    }
    else
    {
      std::cerr << "not of Logged type or does not have PingServer!\n";
    }
    
    if (modem.getPingQueue().size() == 0)
    {
      std::cerr << "Modem queue is empty!\n";
      break;
    }
    
    if (modem.getPingQueuePtr().size() == 0)
    {
      std::cerr << "Modem queue ptr is empty!\n";
      break;
    }
    
    assert(1 == modem.getPingQueue().size());
    PASSED_M("Modem Q has 1 element.");
    
    assert(1 == modem.getPingQueuePtr().size());
    PASSED_M("Modem Q ptr has 1 element.");
    
    for (auto q : modem.getPingQueue())
    {
      std::cout << q.recv_tm << " : " << q.recv_ack << std::endl;
    }
    
    for (auto q : modem.getPingQueuePtr())
    {
      q->execute();
    }
  }
 
}



int main()
{
  variousTests();
  std::cout << "====================================================\n";
  test_Logged_policy();
  std::cout << "====================================================\n";
  test_isInstanceOf_negative();
  std::cout << "====================================================\n";
  test_copy();
  std::cout << "====================================================\n";
  test_ping();
  std::cout << "====================================================\n";
  test_cmptm();
  std::cout << "====================================================\n";
  test_decode_ping();
  std::cout << "====================================================\n";
  /*
  // test_decode_ping_wrong_td();
  // std::cout << "====================================================\n";
  test_decode_ping_new_queue();
  std::cout << "====================================================\n";
  */
  return 0;
}