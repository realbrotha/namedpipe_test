#include "NamedPipeManager.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  std::cout << "Server Main\n";

  NamedPipeManager named_pipe_manager;
  named_pipe_manager.Initialize(1, true);
}