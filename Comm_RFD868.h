#ifndef __RFD900_STUBS_H__
#define __RFD900_STUBS_H__

#include <vector>

class Comm_RFD868
{
  public:
    void addToPingQueue(t_PingInfo&& pingResp)
    {
      pingQueue.push_back(pingResp);
    }
    
    std::vector<t_PingInfo>& getPingQueue()
    {
      return pingQueue;
    }
  
  private:
    std::vector<t_PingInfo> pingQueue;
};

#endif