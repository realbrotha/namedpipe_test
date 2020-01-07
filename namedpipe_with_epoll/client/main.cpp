#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>
#include "NamedPipeWrapper.h"

#include <errno.h>
int main(int argc, char* argv[])
{
    std::cout << "Client \n";
  int type = 1;
  std::string base_path = "/tmp/svr" + std::to_string(type);;

  printf ("pipe path : %s\n",base_path.c_str());
  umask(0000);
  std::string write_pipe_path;

  std::cout << "pipe write string : " << write_pipe_path << "\n";
  int pipe = 0;
  if (NamedPipeWrapper::connect(base_path, pipe, false, true)) {
    std::cout << "read pipe connect success !!!!!!!!!\n";
  } else {
    std::cout << "read pipe connect failed. : " << errno << std::endl;
    perror("error : ");
    return false;
  }
  while (1)
  {
    char buff[1024] = { 0,};
    scanf("%s",buff);
    std::string send(buff);
    if (send == "exit") {
      NamedPipeWrapper::disconnect(pipe);
      printf ("exit!!!!!\n");
      break;
    }
    NamedPipeWrapper::send(pipe, send);
  }
}