#include "PiWatcher_Server.h"
#include "Restart_Server.h"
// #include "Cmd.h"

#include "ComServerContainer.h"
#include "ComManager.h"
#include "ComQueueContainer.h"

#include <vector>
#include <memory> 
#include <iostream>

int main()
{
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

  ComQueueContainer qContainer;
  ComServerContainer sContainer{p_server, p_restart_server};

  ComManager cmdManager{sContainer, qContainer};

  auto p = std::make_shared<cmd::Cmd<Restart_Server>>(2,1);
  qContainer.push_back(p);
  // qContainer.get_CmdQ().push_back(p);

  cmd::Cmd<Restart_Server> smen{1,1};
  std::cout << (int)smen.getC_ID() << '\n';
  smen = cmd::Cmd<Restart_Server>{1,2};
  std::cout << (int)smen.getC_ID() << '\n';

  qContainer.get_CmdQ().clear();
  unsigned char buffer[256];
  buffer[0] = cmd_format::t_ServiceId::SERViCE_COMMAND_DISTRIBUTION;
  buffer[1] = cmd_format::t_CmdId::COMMAND_IMMEDIATE_REBOOT;
  cmdManager.getByteStream(&buffer[0], sizeof(buffer));
  int res = cmdManager.parseCommand();
  if (res < 0)
  {
    std::cerr << "parse failed with res: " << res << std::endl;
  }

  std::cout << qContainer.get_CmdQ().size() << '\n';
  def::GenCmdPtr_t gen_cmd_ptr = qContainer.get_CmdQ().front();
  gen_cmd_ptr->execute();
  gen_cmd_ptr = nullptr;

  buffer[0] = cmd_format::t_ServiceId::SERViCE_COMMAND_DISTRIBUTION;
  buffer[1] = cmd_format::t_CmdId::COMMAND_WATCHDOG;
  buffer[2] = cmd_format::t_watchdog::REG_WATCHDOG_WATCH;
  buffer[3] = static_cast<unsigned char>(60);
  cmdManager.getByteStream(&buffer[0], sizeof(buffer));
  res = cmdManager.parseCommand();  
  if (res < 0)
  {
    std::cerr << "parse failed with res: " << res << std::endl;
  }
  std::cout << qContainer.get_CmdQ().size() << '\n';
  gen_cmd_ptr = qContainer.get_CmdQ().back();
  gen_cmd_ptr->execute();


  std::fill(buffer, buffer+sizeof(buffer), 0U);
  qContainer.get_CmdQ().clear();
  buffer[0] = cmd_format::t_ServiceId::SERViCE_COMMAND_DISTRIBUTION;
  buffer[1] = cmd_format::t_CmdId::COMMAND_WATCHDOG;
  buffer[2] = cmd_format::t_watchdog::REG_WATCHDOG_WATCH;
  buffer[3] = static_cast<unsigned char>(60);
  buffer[4] = cmd_format::CMD_SEPARATOR;
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
  std::cout << qContainer.get_CmdQ().size() << '\n';

  for (auto p : qContainer.get_CmdQ())
  {
    p->execute();
  }

  // cmd::Cmd<Restart_Server, policies::ExecAtEndOfCycle> polCmd{2,1};
  auto p_polCmd = std::make_shared<cmd::Cmd<Restart_Server, policies::ExecAtEndOfCycle>>(2,1);
  p_polCmd->init(p_restart_server, &Restart_Server::immediatePiRestart, {});
  p_polCmd->execute();

  qContainer.get_CmdQ().clear();
  qContainer.push_back(p_polCmd, def::t_CmdTypes::e_CmdEndOfCycle);

  return 0;
}
