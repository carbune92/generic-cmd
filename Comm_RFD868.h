#ifndef __RFD900_STUBS_H__
#define __RFD900_STUBS_H__

#include <vector>
#include "Cmd.h"

class Comm_RFD868
{
  public:
    void addToPingQueue(t_PingInfo&& pingResp)
    {
      pingQueue.push_back(pingResp);
    }
    
    void addToPingQueue(def::GenCmdPtr_t p)
    {
      pingQueuePtr.push_back(p);
    }
    
    std::vector<t_PingInfo>& getPingQueue()
    {
      return pingQueue;
    }
    
    std::vector<def::GenCmdPtr_t>& getPingQueuePtr()
    {
      return pingQueuePtr;
    }
  
  private:
    std::vector<t_PingInfo> pingQueue;
    std::vector<def::GenCmdPtr_t> pingQueuePtr;
};

#endif