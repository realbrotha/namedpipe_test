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

int main(int argc, char *argv[]) {
  int code = ProductList::kEZREAL | ProductList::kSORAKA | ProductList::kASHE;

  NamedPipeManager server;
  bool result = server.Initialize( ProductList::kEZREAL, true);
  std::cout << "init ipc result : " << result << "\n";

  char buffer[100] = {0,};
  memset(buffer, 0x00, sizeof(buffer));

  while (1) {
    scanf("%s", &buffer);
    std::string buff(buffer);
    server.SendData( ProductList::kEZREAL, buff);
    if (buff == "quit")
    {
      std::cout <<"QUIT!!!!!!!!!!!!!!!!!!!";
      break;
    }
  }
  // for test code
  // Not Yet
}