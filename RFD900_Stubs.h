#ifndef __RFD900_STUBS_H__
#define __RFD900_STUBS_H__

#include <vector>

class RFD900_Modem
{
  public:
    void addToPingQueue(t_PingInfo&& pingResp)
    {
      pingQueue.push_back(pingResp);
    }
    
    const std::vector<t_PingInfo>& getPingQueue() const
    {
      return pingQueue;
    }
  
  private:
    std::vector<t_PingInfo> pingQueue;
};

#endif