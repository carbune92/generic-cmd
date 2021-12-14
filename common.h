#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <vector>
#include <functional>
#include <memory>

// namespace policies
// {
//   class DeallocMemPrecond;
// }

namespace cmd
{
  class Generic_Cmd;

  // template <typename T>
  // class Cmd<T, policies::DeallocMemPrecond>;
  // class Cmd;
}

// #include "Cmd.h"

namespace def
{
  typedef uint8_t t_SID;
  typedef uint8_t t_CID;

  using data_t = std::vector<uint8_t>;

  template <typename T>
  using Call_t = std::function<data_t(T, data_t)>;

  using GenCmdPtr_t = std::shared_ptr<cmd::Generic_Cmd>;

  // template <typename T>
  // using EndofCycleCmdPtr_t = std::shared_ptr<cmd::Cmd<T, policies::ExecAtEndOfCycle>>;

  // template <typename T>
  // using DeallocMemCmdPtr_t = std::shared_ptr<cmd::Cmd<T, policies::DeallocMemPrecond>>;

  enum t_CmdTypes
  {
    e_CmdDefault = 0,
    e_CmdEndOfCycle,
    e_CmdDeallocMem,
    e_Logged,
    /* reserved */
    e_cmd1,
    e_cmd2
  };
};

namespace cmd_format
{

  static const uint8_t CMD_SEPARATOR = '|';

  namespace t_ServiceId
  {
    static const uint8_t SERViCE_COMMAND_DISTRIBUTION = 2;
    // static const uint8_t RESERVED = 3;
  };

  namespace t_CmdId
  {
    static const uint8_t COMMAND_IMMEDIATE_REBOOT = 1;
    static const uint8_t COMMAND_ONOFF_SENSOR = 2;
    static const uint8_t COMMAND_ONOFF_IRIDIUM = 3;
    static const uint8_t COMMAND_WATCHDOG = 4;
  };

  namespace t_restart
  {
    static constexpr int NR_BYTES_PARAMS_RESTART = 0;
  };

  namespace t_watchdog
  {
    static const uint8_t REG_WATCHDOG_WATCH = 1;
    static const uint8_t REG_WATCHDOG_WAKE = 2;
    static const uint8_t REG_WATCHDOG_DUMP = 8;

    static constexpr int NR_BYTES_PARAMS_WATCH = sizeof(uint8_t) + sizeof(uint8_t);
    static constexpr int NR_BYTES_PARAMS_WAKE = sizeof(uint8_t) + sizeof(uint16_t);
    static constexpr int NR_BYTES_PARAMS_DUMP = sizeof(uint8_t);
  };
};

#endif