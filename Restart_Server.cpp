#include "Restart_Server.h"

#include <iostream>
#include <cstdlib>

void Restart_Server::setup()
{
  std::cout << "Restart_Server::setup" << '\n';
}

def::data_t Restart_Server::immediatePiRestart(def::data_t req)
{
  std::cout << "Restart_Server::immediatePiRestart" << '\n';
  system("sudo reboot -h now");
  return def::data_t{};
}
