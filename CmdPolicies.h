#ifndef __CMDPOLICIES_H__
#define __CMDPOLICIES_H__

#include <iostream>

namespace cmd
{
  class Generic_Cmd;
}

namespace policies
{
  class ExecAtEndOfCycle : virtual public cmd::Generic_Cmd
  {
    public:
    virtual ~ExecAtEndOfCycle() = default;
  };

  class DeallocMemPrecond : virtual public cmd::Generic_Cmd
  {
    public:
    virtual void dealloc() { std::cout << "dealloc\n"; }
    virtual ~DeallocMemPrecond() = default;
  };
}

#endif