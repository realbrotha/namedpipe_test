#include "NamedPipeServer.h"

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

  bool result = NamedPipeServer::GetInstance().Initialize(code);
  std::cout << "init ipc result : " << result << "\n";

  int i = 0;
  while (1) {
    std::cout << "tick tock\n";
    if (++i == 3)
      NamedPipeServer::GetInstance().RemovePipeThreads(ProductList::kSORAKA);

    sleep(1);
  }
  // for test code
  // Not Yet
}