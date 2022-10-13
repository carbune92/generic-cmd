#include "Ping_Server.h"
#include <iostream>

#include "Cmd.h"

#include "Comm_RFD868.h"

bool cmptm(const std::string& tm1, const std::string& tm2)
{
  static const std::string dt_format{"%d-%m-%Y %H-%M-%S"};
  
  std::istringstream ss1{tm1}, ss2{tm2};
  std::tm dt1, dt2;

  ss1 >> std::get_time(&dt1, dt_format.c_str());
  ss2 >> std::get_time(&dt2, dt_format.c_str());
  
  double diff = difftime(mktime(&dt1),mktime(&dt2));
  
  return (diff <= 0.0)? true : false;
}

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
  std::pair<std::string,int> res = req2pair(req);
  
  addToQueue(res.first, res.second);
  
  return def::data_t();
}

std::string PingServer::req2str(def::data_t req) const
{
  std::string res;
  
  auto pair = req2pair(req);
  std::string tm = pair.first;
  int ack = pair.second;

  // std::string gmtTdStr = strTD_local2gmt(tm);
  
  res = "ping cmd recv at (" + tm + ") with ack = " + std::to_string(ack);

  return res;
}

void PingServer::addToQueue(std::string recv_tm, int recv_ack)
{
  std::cout << "recv_tm: " << recv_tm << ", recv_ack: " << recv_ack << std::endl;
  if (recv_ack >= 0 && recv_ack < 1000)
  { 
    t_PingInfo pingInfo{recv_tm, recv_ack};
    // bool res = timedateOfPingResp(pingInfo);
    // std::cout << " res of timedateOfPingResp: " << (int)res << std::endl;
    if (true)
    {
      // std::cout << "Ping cmd timedate is actual!\n";
      modem.addToPingQueue( std::move(pingInfo) );

      std::shared_ptr<cmd::Cmd<PingServer>> p_pingResp =
        std::make_shared<cmd::Cmd<PingServer>>(cmd_format::t_ServiceId::DIAGNOSTICS, cmd_format::t_CmdId::COMMAND_PING);
 
      std::shared_ptr<PingServer> p_pingSrv = std::make_shared<PingServer>(modem);
      
      def::data_t updated_req = serialize2req(pingInfo);
      p_pingResp->init(p_pingSrv, &PingServer::sendResponse, updated_req);
      p_pingSrv = nullptr;
      
      modem.addToPingQueue(p_pingResp);
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
  auto pair = req2pair(req);
  
  std::string toSend = serialize2modemstr({pair.first, pair.second});
  
  // std::cout << "serialized ping response to send: " << toSend << std::endl;
  std::cout << "[PING_CMD] : serialized ping response to send: " << toSend << std::endl;
  std::cout << "[PING CMD] :" << pair.first << " : " << pair.second << std::endl;
  
  // send via modem
  modem.sendPingData({pair.first, pair.second});
  
  return def::data_t{};
}

bool PingServer::timedateOfPingResp(t_PingInfo& p)
{
  bool res = false;
  
  // std::string crtGmtTd = strTD_getGmtTime();

  // std::string gmtTdStr = strTD_local2gmt(p.recv_tm);
  
  // res = cmptm(gmtTdStr, crtGmtTd);
  
  // if (res)
  // {
  //   p.recv_tm = crtGmtTd;
  // }
  
  return res;
}

std::string PingServer::serialize2modemstr(t_PingInfo&& p)
{
  std::string res = "";

  // "ping|S_ID|C_ID|timedate|ack"

  static char outstr[64];
  sprintf(outstr, "ping|%d|%d|%s|%d\n", 
    (int)cmd_format::t_ServiceId::DIAGNOSTICS, 
    (int)cmd_format::t_CmdId::COMMAND_PING, 
    p.recv_tm.c_str(), 
    p.recv_ack);

  res += outstr;

  return res;
}

def::data_t PingServer::serialize2req(const t_PingInfo& p)
{
  def::data_t req{};
  
  for (const char& c : p.recv_tm)
  {
    if (!isdigit((int)c)) continue;
    req.push_back(static_cast<uint8_t>(c));
  }
  
  int ack = p.recv_ack;
  req.push_back( static_cast<uint8_t>( (ack >> 8) & 0x000000FF ) );
  req.push_back( static_cast<uint8_t>( ack & 0x000000FF ) );
  
  return req;
}

std::pair<std::string, int> PingServer::req2pair(def::data_t req)
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
  
  int ack = static_cast<int>(static_cast<int>(req[14] << 8) | req[15]);
  
  return {tm, ack};
}
