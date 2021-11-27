#include "PiWatcher_Server.h"

#include <iostream>

#include <errno.h>
#include <cstring>

void PiWatcherServer::setup()
{
  if (-1 == m_fd)
  {
    int ret = wiringPiI2CSetup(m_i2c_dev);

    if (ret < 0)
    {
      std::cout << "Error PiWatcherServer::setup : " << strerror(errno) << std::endl;
    }
    else
    {
      m_fd = ret;
    }
  }
}

def::data_t PiWatcherServer::status(def::data_t req)
{
  def::data_t resp;

  if (-1 != m_fd)
  {
    unsigned char reg = req[0];
    int ret = wiringPiI2CReadReg8(m_fd, static_cast<int>(reg));

    if (ret >= 0)
    {
      unsigned char len = 1U;
      resp.push_back(len);
      resp.push_back( static_cast<unsigned char>(ret & 0xFF) );
    }
    else
    {
      std::cout << "PiWatcherServer::status wiringPiI2CReadReg8 failed with code " << errno << " and message: " << strerror(errno) << std::endl;
    }
  }
  else
  {
    std::cout << "PiWatcherServer::status i2c fd is not initialized!" <<std::endl;
  }

  return resp;
}

def::data_t PiWatcherServer::watch(def::data_t req)
{
  def::data_t resp;

  if (-1 != m_fd)
  {
    int reg = static_cast<int> (req[0]);
    int sec = static_cast<int> (req[1]);

    int ret = wiringPiI2CWriteReg8(m_fd, reg, sec);

    if (ret == 0)
    {
      unsigned char len = 1U;

      resp.push_back(len);
      resp.push_back( static_cast<unsigned char>(ret & 0xFF) );
    }
    else
    {
      std::cout << "PiWatcherServer::watch wiringPiI2CWriteReg8 failed(" << ret << ") with code " << errno << " and message: " << strerror(errno) << std::endl;
    }
  }
  else
  {
    std::cout << "PiWatcherServer::watch i2c fd is not initialized!" <<std::endl;
  }

  return resp;
}

def::data_t PiWatcherServer::wake(def::data_t req)
{
  def::data_t resp;

  if (-1 != m_fd)
  {
    int reg = static_cast<int> (req[0]);
    int sec = static_cast<int>(((req[1] << 8) | static_cast<int>(req[2])) & 0x0000FFFF);
    int ret = wiringPiI2CWriteReg16(m_fd, reg, sec);

    if (ret == 0)
    {
      unsigned char len = 1U;

      resp.push_back(len);
      resp.push_back( static_cast<unsigned char>(ret & 0xFF) );
    }
    else
    {
      std::cout << "PiWatcherServer::watch wiringPiI2CWriteReg16 failed(" << ret << ") with code " << errno << " and message: " << strerror(errno) << std::endl;  
    }
  }
  else
  {
    std::cout << "PiWatcherServer::wake i2c fd is not initialized!" <<std::endl;
  }

  return resp;
}

def::data_t PiWatcherServer::dump(def::data_t req)
{
  def::data_t resp;
  (void)req;

  if (-1 != m_fd)
  {
    unsigned char len = 8U;
    resp.push_back(len);

    for (int i = 0; i < 8; i++)
    {
      resp.push_back( static_cast<unsigned char>(wiringPiI2CReadReg8(m_fd,i) & 0xFF) );
    }
  }
  else
  {
    std::cout << "PiWatcherServer::dump i2c fd is not initialized!" <<std::endl;
  }

  return resp;
}