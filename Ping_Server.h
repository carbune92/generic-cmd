#ifndef __PING_SERVER_H__
#define __PING_SERVER_H__

#include "common.h"

class PingServer
{
  public:
    void setup();
    
  private:
    int m_ack = -1;
    
};

#endif