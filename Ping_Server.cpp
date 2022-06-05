#include "Ping_Server.h"
#include <iostream>

bool cmptm(const std::string& tm1, const std::string& tm2)
{
  const std::string dt_format{"%d-%m-%Y %H-%M-%S"};
  
  std::istringstream ss1{tm1}, ss2{tm2};
  std::tm dt1, dt2;
  
  ss1 >> std::get_time(&dt1, dt_format.c_str());
  ss2 >> std::get_time(&dt2, dt_format.c_str());
  
  double diff = difftime(mktime(&dt1),mktime(&dt2));
  
  return (diff < 0.0)? true : false;
}

PingServer::PingServer(RFD900_Modem& _modem) :
  modem{_modem},
  m_ackTable{cmptm}
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
  
  int ack = static_cast<int>(static_cast<int>(req[14] << 24) | (req[15] << 16) | (req[16] << 8) | req[17]);
  
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
  
  int ack = static_cast<int>(static_cast<int>(req[14] << 24) | (req[15] << 16) | (req[16] << 8) | req[17]);
  
  res = "ping cmd recv at (" + tm + ") with ack = " + std::to_string(ack);
  
  return res;
}

void PingServer::addToQueue(std::string recv_tm, int recv_ack)
{
  if (recv_ack >= 0 && recv_ack < 1000)
  {  
    t_PingInfo pingInfo{recv_tm, recv_ack};
    modem.addToPingQueue( std::move(pingInfo) );
    
    m_ackTable.insert( {recv_tm, recv_ack} );
  } 
  else
  {
    //TODO the proper reaction - maybe send a response with ack -1
  }
}