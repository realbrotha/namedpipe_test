#include "UnixDomainSocketInterface.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  std::cout << "Server Main\n";

  UnixDomainSocketInterface impl;
  UnixDomainSocketServer *server;

  bool result = impl.GetInterfaceObject(kINTERFACE_TYPE_SERVER, reinterpret_cast<UnixDomainSocketInterface **>(&server));

  if (result && server->Initialize()) {
    for (;;) {
      char buffer[50] = {0,};
      std::cin >> buffer;
      std::string buffer2(buffer);
      server->SendMessageBroadcast(buffer2);
    }
  } else {
    std::cout << "server init failed";
  }
  std::cout << "Lead Fin";
}
