#ifndef __LOGGED_H__
#define __LOGGED_H__

#include <string>
#include <iomanip>
#include <ctime>
#include <fstream>
#include "Generic_Cmd.h"

namespace policies
{
  class Logged : virtual public cmd::Generic_Cmd
  {
    public:
    virtual void setFileStream(const char* filename);
    virtual void log(std::string context);

    virtual ~Logged();

    private:
    std::ofstream m_out;
  };
}

#endif