#ifndef __PING_SERVER_H__
#define __PING_SERVER_H__

#include "common.h"

class PingServer
{
  public:
    void setup();

    //TODO inject dependencies for RDF module for sending
    
    
  private:
    int m_ack = -1;
    
};

#endif