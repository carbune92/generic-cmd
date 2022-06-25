#include "Ping_Server.h"
#include <iostream>

#include "Cmd.h"

bool cmptm(const std::string& tm1, const std::string& tm2)
{
  const std::string dt_format{"%d-%m-%Y %H-%M-%S"};
  
  std::istringstream ss1{tm1}, ss2{tm2};
  std::tm dt1, dt2;

  ss1 >> std::get_time(&dt1, dt_format.c_str());
  ss2 >> std::get_time(&dt2, dt_format.c_str());
  
  double diff = difftime(mktime(&dt1),mktime(&dt2));
  
  return (diff <= 0.0)? true : false;
}


// std::shared_ptr<PingServer> PingServer::create(Comm_RFD868& _modem)
// {
//   return std::shared_ptr<PingServer>(new PingServer(_modem));
// }

PingServer::PingServer(Comm_RFD868& _modem) :
  modem{_modem}
  // m_ackTable{cmptm}
{
}

void PingServer::setup()
{
}

def::data_t PingServer::addToModemQueue(def::data_t req)
{
  // datetime format %d-%m-%Y %H-%M-%S
  /*
    %d - 01-31(2B)
    %m - 01-12(2B)
    %Y (4B) OR %y (2B)
    %H - 00-23 (2B)
    %M - 00-59 (2B)
    %S - 00-59 (2B)
  ----------------------
    12B OR 14B
  */
  std::string tm = "";
  tm += std::string(1,req[0]) + (char)req[1];
  tm += '-';
  tm += std::string(1,req[2]) + (char)req[3];  
  tm += '-';
  tm += std::string(1,req[4]) + (char)req[5] + (char)req[6] + (char)req[7];
  tm += ' ';
  tm += std::string(1,req[8]) + (char)req[9];
  tm += '-';
  tm += std::string(1,req[10]) + (char)req[11];
  tm += '-';
  tm += std::string(1,req[12]) + (char)req[13];
  
  // int ack = static_cast<int>(static_cast<int>(req[14] << 24) | (req[15] << 16) | (req[16] << 8) | req[17]);
  
  int ack = static_cast<int>(static_cast<int>(req[14] << 8) | req[15]);
  
  addToQueue(tm, ack);
  
  return def::data_t();
}

std::string PingServer::req2str(def::data_t req) const
{
  std::string res;
  
  std::string tm = "";
  tm += std::string(1,req[0]) + (char)req[1];
  tm += '-';
  tm += std::string(1,req[2]) + (char)req[3];  
  tm += '-';
  tm += std::string(1,req[4]) + (char)req[5] + (char)req[6] + (char)req[7];
  tm += ' ';
  tm += std::string(1,req[8]) + (char)req[9];
  tm += '-';
  tm += std::string(1,req[10]) + (char)req[11];
  tm += '-';
  tm += std::string(1,req[12]) + (char)req[13];
  
    
  // int ack = static_cast<int>(static_cast<int>(req[14] << 24) | (req[15] << 16) | (req[16] << 8) | req[17]);
  
  int ack = static_cast<int>(static_cast<int>(req[14] << 8) | req[15]);
  
  // res = "ping cmd recv at (" + tm + ") with ack = " + std::to_string(ack);


   const char dt_format[] {"%d-%m-%Y %H-%M-%S"};
  std::istringstream ss{tm};
  std::tm dt;
  ss >> std::get_time(&dt, dt_format);
  const time_t tmp = mktime(&dt);
  std::tm * p_dt_gmt = gmtime(&tmp);
  std::ostringstream oss0;
  oss0 << std::put_time(p_dt_gmt, dt_format);
  
  res = "ping cmd recv at (" + oss0.str() + ") with ack = " + std::to_string(ack);

  return res;
}

void PingServer::addToQueue(std::string recv_tm, int recv_ack, def::data_t req)
{
  std::cout << "recv_tm: " << recv_tm << ", recv_ack: " << recv_ack << std::endl;
  if (recv_ack >= 0 && recv_ack < 1000)
  { 
    t_PingInfo pingInfo{recv_tm, recv_ack};
    bool res = timedateOfPingResp(pingInfo);
    std::cout << " res of timedateOfPingResp: " << (int)res << std::endl;
    if (res)
    {
      // std::cout << "Ping cmd timedate is actual!\n";
      modem.addToPingQueue( std::move(pingInfo) );

      std::shared_ptr<cmd::Cmd<PingServer>> p_pingResp =
        std::make_shared<cmd::Cmd<PingServer>>(cmd_format::t_ServiceId::DIAGNOSTICS, cmd_format::t_CmdId::COMMAND_PING);
 
      std::shared_ptr<PingServer> p_pingSrv = std::make_shared<PingServer>(modem);
      p_pingResp->init(p_pingSrv, &PingServer::sendResponse, req);
      p_pingSrv = nullptr;

      // m_ackTable.insert( {recv_tm, recv_ack} );
    }
    else
    {
      // TODO update error timedate greater that initial timedate!
    }  
  } 
  else
  {
    //TODO the proper reaction - maybe send a response with ack -1
  }
}

def::data_t PingServer::sendResponse(def::data_t req)
{
  return def::data_t{};
}

// std::shared_ptr<PingServer> PingServer::getptr()
// {
//   return shared_from_this();
// }

bool PingServer::timedateOfPingResp(t_PingInfo& p)
{
  bool res = false;
  
  std::time_t t = std::time(nullptr);
  const char dt_format[] {"%d-%m-%Y %H-%M-%S"};
  
  std::ostringstream oss;
  std::tm *tm = std::gmtime(&t);
  oss << std::put_time(tm, dt_format);

/*  
  std::istringstream ss1{tm1}, ss2{tm2};
  std::tm dt1, dt2;

  ss1 >> std::get_time(&dt1, dt_format.c_str());
  ss2 >> std::get_time(&dt2, dt_format.c_str());
  
  double diff = difftime(mktime(&dt1),mktime(&dt2));
*/

  std::istringstream ss{p.recv_tm};
  std::tm dt;
  ss >> std::get_time(&dt, dt_format);
  const time_t tmp = mktime(&dt);
  std::tm * p_dt_gmt = gmtime(&tmp);
  std::ostringstream oss0;
  oss0 << std::put_time(p_dt_gmt, dt_format);
  
  res = cmptm(oss0.str(), oss.str());
  
  if (res)
  {
    p.recv_tm = oss.str();
  }
  
  return res;
}

std::string PingServer::serialize(const t_PingInfo& p)
{
  std::string res = "";

  // "ping|S_ID|C_ID|timedate|ack"

  char outstr[25];
  sprintf(outstr, "ping|%d|%d|%s|%d\n", 
    (int)cmd_format::t_ServiceId::DIAGNOSTICS, 
    (int)cmd_format::t_CmdId::COMMAND_PING, 
    p.recv_tm.c_str(), 
    p.recv_ack);

  res += outstr;

  return res;
}