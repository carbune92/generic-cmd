#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <vector>
#include <functional>
#include <memory>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>

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

/*
inline std::string strTD_local2gmt(std::string localstr)
{
  std::string gmtstr;
  
  static const char dt_format[] {"%d-%m-%Y %H-%M-%S"};
  std::istringstream ss{localstr};
  std::tm dt;
  ss >> std::get_time(&dt, dt_format);
  const time_t tmp = mktime(&dt);
  std::tm * p_dt_gmt = gmtime(&tmp);
  std::ostringstream oss;
  oss << std::put_time(p_dt_gmt, dt_format);
  gmtstr = oss.str();
  
  return gmtstr;
}


inline std::string strTD_gmt2local(std::string gmtstr)
{
  std::string localstr;
  
  static const char dt_format[] {"%d-%m-%Y %H-%M-%S"};
  std::istringstream ss{gmtstr};
  std::tm dt;
  ss >> std::get_time(&dt, dt_format);
  const time_t tmp = mktime(&dt);
  std::tm * p_dt_gmt = localtime(&tmp);
  std::ostringstream oss;
  oss << std::put_time(p_dt_gmt, dt_format);
  localstr = oss.str();
  
  return localstr;
}

inline std::string strTD_getGmtTime()
{ 
  std::time_t t = std::time(nullptr);
  static const char dt_format[] {"%d-%m-%Y %H-%M-%S"};
  
  std::ostringstream oss;
  std::tm *tm = std::gmtime(&t);
  oss << std::put_time(tm, dt_format);
  
  return oss.str();
}

inline std::string strTD_getLocalTime()
{ 
  std::time_t t = std::time(nullptr);
  static const char dt_format[] {"%d-%m-%Y %H-%M-%S"};
  
  std::ostringstream oss;
  std::tm *tm = std::localtime(&t);
  oss << std::put_time(tm, dt_format);
  
  return oss.str();
}
*/

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
    e_Ping,
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
    static const uint8_t DIAGNOSTICS = 3;
  };

  namespace t_CmdId
  {
    static const uint8_t COMMAND_IMMEDIATE_REBOOT = 1;
    static const uint8_t COMMAND_ONOFF_SENSOR = 2;
    static const uint8_t COMMAND_ONOFF_IRIDIUM = 3;
    static const uint8_t COMMAND_WATCHDOG = 4;
    static const uint8_t COMMAND_PING = 1;
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
  
  namespace t_ping
  {
    static constexpr int NR_BYTES_ACK = sizeof(uint16_t);
    static constexpr int NR_BYTES_TD = 14 * sizeof(uint8_t);
  }
};

struct t_PingInfo
{
  std::string recv_tm;
  int recv_ack;
};

#endif