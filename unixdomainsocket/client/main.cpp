#include "UnixDomainSocketInterface.h"

#include <iostream>

UnixDomainSocketClient *client;

void callbackFunc (Message& msg)
{
  printf ("callback !!!!!\n");
  MessageParser parse(msg);
  printf ("callback : message size : %d\n", parse.GetDataSize());

  std::string data = "hehe ok response";
  int msgId = parse.GetMessageId();

  if (parse.GetDataSize() == 5) {
    printf ("reponse 간드아!!!\n");
    std::string data = "respone 간드아!!";
    client->SendMessage(msgId, data);
  }
}

int main(int argc, char *argv[]) {
  std::cout << "client";

  UnixDomainSocketInterface impl;


  std::string buffer3;
  for (int i = 0; i < 1000 *100; ++i) {
    buffer3 += "abcde12345";
  }
  printf ("last key [%c] [%c] [%c]\n", buffer3[buffer3.size()], buffer3[buffer3.size() -1], buffer3[buffer3.size()-2]);
  bool result = impl.GetInterfaceObject(kINTERFACE_TYPE_CLIENT, reinterpret_cast<UnixDomainSocketInterface **>(&client));

  int product_code = 0x01;
  if (result && client->Initialize(product_code, callbackFunc)) {
    for (;;) {
      char buffer[50] = {0,};
      std::cin >> buffer;
      std::string buffer2(buffer);
      if (buffer2 == "big") {
        std::cout << "big!!!! send" << std::endl;
        client->SendMessage(buffer3);
      } else {
        client->SendMessage(buffer2);
      }
    }
  } else {
    std::cout << "client init failed";
  }

}