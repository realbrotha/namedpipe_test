#include "NamedPipeManager.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

enum ProductList // for test
{
  kEZREAL = 1,
  kSORAKA = 2,
  kASHE = 4
};

int main(int argc, char* argv[])
{
  int code = ProductList::kEZREAL;

  NamedPipeManager client;
  bool result = client.Initialize(code, false);
  std::cout << "init ipc result : " << result << "\n";

  int i = 0;
  char buffer[100] = {0,};
  memset(buffer, 0x00, sizeof(buffer));

  while (1) {
    scanf("%s", &buffer);
    std::string buff(buffer);
    std::cout << "read string data : " << buff << "\n";
    std::cout << "send!!!!!!!!!!! : " << client.SendData(code, buff) << "\n";
    if (buff == "quit")
    {
      std::cout <<"QUIT!!!!!!!!!!!!!!!!!!!";
      break;
    }
  }
}