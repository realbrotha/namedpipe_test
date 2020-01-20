#include "UnixDomainSocketInterface.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  std::cout << "Server Main\n";

  UnixDomainSocketInterface impl;
  UnixDomainSocketServer *server;

  bool
      result = impl.GetInterfaceObject(kINTERFACE_TYPE_SERVER, reinterpret_cast<UnixDomainSocketInterface **>(&server));

  std::string buffer3;
  for (int i = 0; i < 1000 * 100; ++i) {
    buffer3 += "abcde12345";
  }
  if (result && server->Initialize()) {
    for (;;) {
      char buffer[50] = {0,};
      std::cin >> buffer;
      std::string buffer2(buffer);

      if (buffer2 == "big") {
        std::cout << "big!!!! send" << std::endl;
        server->SendMessageBroadcast(buffer3);
      } else if (buffer2 == "5") {
        printf ("!!!!!!!!!!!!!!!!!!wait send!!!!!!!!!!!");
        std::string test = "abcde";
        std::string result ;
        result = server->SendMessage(test, 1, 0xffffff);
        printf ("result : %s", result.c_str());
      } else {
        server->SendMessageBroadcast(buffer2);
      }

    }
  } else {
    std::cout << "server init failed";
  }
  impl.Release();
  std::cout << "Lead Fin";
}
