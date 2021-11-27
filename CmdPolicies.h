#ifndef __CMDPOLICIES_H__
#define __CMDPOLICIES_H__

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
    virtual ~DeallocMemPrecond() = default;
  };
}

#endif