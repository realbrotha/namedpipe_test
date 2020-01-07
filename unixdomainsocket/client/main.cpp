#include "UnixDomainSocketInterface.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  std::cout << "client";

  UnixDomainSocketInterface impl;
  UnixDomainSocketClient *client;

  bool result = impl.GetInterfaceObject(kINTERFACE_TYPE_CLIENT, reinterpret_cast<UnixDomainSocketInterface **>(&client));
  if (result && client->Initialize()) {
    for (;;) {
      char buffer[50] = {0,};
      std::cin >> buffer;
      std::string buffer2(buffer);
      client->SendMessage(buffer2);
    }
  } else {
    std::cout << "client init failed";
  }

}