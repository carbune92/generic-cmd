#include "Logged.h"
#include <iostream>

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

  Logged::~Logged()
  {
    m_out.close();
  }
}