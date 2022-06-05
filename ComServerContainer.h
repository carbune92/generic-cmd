#ifndef __COMSERVERCONTAINER_H__
#define __COMSERVERCONTAINER_H__

#include "PiWatcher_Server.h"
#include "Restart_Server.h"
#include "Ping_Server.h"
#include <memory>

class ComServerContainer
{
  public:
  explicit ComServerContainer(
    std::shared_ptr<PiWatcherServer> piwatcher_server,
    std::shared_ptr<Restart_Server> restart_server,
    std::shared_ptr<PingServer> ping_server
  ); 

  ComServerContainer();

  void init(
    std::shared_ptr<PiWatcherServer> piwatcher_server,
    std::shared_ptr<Restart_Server> restart_server,
    std::shared_ptr<PingServer> ping_server
  );

  std::shared_ptr<PiWatcherServer> get_Piwatcher_server();
  std::shared_ptr<Restart_Server> get_Restart_server();
  std::shared_ptr<PingServer> get_Ping_server();

  private:
  std::shared_ptr<PiWatcherServer> m_piwatcher_server;
  std::shared_ptr<Restart_Server> m_restart_server;
  std::shared_ptr<PingServer> m_ping_server;
};

#endif