#include "ComServerContainer.h"

ComServerContainer::ComServerContainer(
      std::shared_ptr<PiWatcherServer> piwatcher_server,
      std::shared_ptr<Restart_Server> restart_server
) : 
m_piwatcher_server{piwatcher_server}, 
m_restart_server{restart_server}
{}

ComServerContainer::ComServerContainer() {}

void ComServerContainer::init(
    std::shared_ptr<PiWatcherServer> piwatcher_server,
    std::shared_ptr<Restart_Server> restart_server
  )
{
  m_piwatcher_server = piwatcher_server;
  m_restart_server = restart_server;
}

std::shared_ptr<PiWatcherServer> ComServerContainer::get_Piwatcher_server()
{
  return m_piwatcher_server;
}

std::shared_ptr<Restart_Server> ComServerContainer::get_Restart_server()
{
  return m_restart_server;
}