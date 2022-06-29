#ifndef __PING_SERVER_H__
#define __PING_SERVER_H__

#include "common.h"
// #include <map>
#include <string>
#include <functional>
#include <utility>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

class Comm_RFD868;

bool cmptm(const std::string&, const std::string&);

// using MapFunctor_t = std::function<bool(std::string,std::string)>;

class PingServer
{
  public:
    PingServer(Comm_RFD868& _modem);
    PingServer() = delete;
    void setup();
    def::data_t addToModemQueue(def::data_t req);
    def::data_t sendResponse(def::data_t req);
    std::string req2str(def::data_t req) const;
    
  private:
    void addToQueue(std::string recv_tm, int recv_ack);
    static bool timedateOfPingResp(t_PingInfo& p);
    static std::string serialize2modemstr(const t_PingInfo& p);
    static def::data_t serialize2req(const t_PingInfo& p);
    static std::pair<std::string, int> req2pair(def::data_t);
    // int m_last_ack = -1;
    // std::map< std::string, 
    //           int,
    //           std::function<bool(const std::string&, const std::string&)>
    //         > m_ackTable;
           
    Comm_RFD868& modem;
    
    
    // TODO server classes:
    /*
      - make an interface for all server classes
      - add in the interface the req2str for logging purposes
      - each command will also return the interface object for its server class so that it can call the 'req2str' method
    */
};

#endif