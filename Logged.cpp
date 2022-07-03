#include "Logged.h"
#include <iostream>

#include "Ping_Server.h"

namespace policies
{
  void Logged::setFileStream(const char* filename)
  {
    if (!m_out.is_open())
    {
      m_out.open(filename, std::ios_base::app);
      if (!m_out)
      {
        std::cerr << "Logged: bad stream!\n";
      }
    }
  }

  void Logged::log(std::string context)
  {
    if (m_out)
    {
      auto t = std::time(nullptr);
      auto tm = *std::localtime(&t);
      m_out << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << ": " << context << std::endl;
    }
  }
  
  void Logged::log(std::shared_ptr<PingServer> pingServ)
  {
    std::string s = pingServ->req2str(this->get_req());
    this->log(s);
  }

  Logged::~Logged()
  {
    m_out.close();
  }
}