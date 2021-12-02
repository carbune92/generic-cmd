#ifndef __PIWATCHER_SERVER_H__
#define __PIWATCHER_SERVER_H__

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "common.h"

#include <array>

#define DEFAULT_PIWATCHER_I2C_DEV       (0x01)
#define DEFAULT_PIWATCHER_I2C_ADDR      (0x62)

class PiWatcherServer
{
  public:
    void setup();
    // void registerCmd(PiWatcherCommand*);

    def::data_t status(def::data_t);
    def::data_t watch(def::data_t);
    def::data_t wake(def::data_t);
    def::data_t dump(def::data_t);
    
  private:
    const int m_i2c_dev = DEFAULT_PIWATCHER_I2C_DEV;
    int m_fd = -1;
};

#endif
