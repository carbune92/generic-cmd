#ifndef __LOGGED_H__
#define __LOGGED_H__

#include <string>
#include <iomanip>
#include <ctime>
#include <fstream>
#include "Generic_Cmd.h"

class PingServer;

namespace policies
{
  class Logged : virtual public cmd::Generic_Cmd
  {
    public:
    void setFileStream(const char* filename);
    void log(std::string context);
    
    virtual void log(std::shared_ptr<PingServer>);

    virtual ~Logged();

    private:
    std::ofstream m_out;
  };
}

#endif