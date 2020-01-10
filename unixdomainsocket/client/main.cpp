#include "UnixDomainSocketInterface.h"

#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << "client";

  UnixDomainSocketInterface impl;
  UnixDomainSocketClient *client;

  std::string buffer3;
  for (int i = 0; i < 1000 * 1000 * 10; ++i) {
    buffer3 += "abcde12345";
  }

  bool result = impl.GetInterfaceObject(kINTERFACE_TYPE_CLIENT, reinterpret_cast<UnixDomainSocketInterface **>(&client));

  if (result && client->Initialize()) {
    for (;;) {
      char buffer[50] = {0,};
      std::cin >> buffer;
      std::string buffer2(buffer);
      if (buffer2 == "big") {
        std::cout << "big!!!! send";
        client->SendMessage(buffer3);
      }
      client->SendMessage(buffer2);
    }
  } else {
    std::cout << "client init failed";
  }

}