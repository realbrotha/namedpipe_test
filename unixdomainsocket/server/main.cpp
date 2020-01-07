#include "UnixDomainSocketInterface.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  std::cout << "Server Main\n";

  UnixDomainSocketInterface impl;
  UnixDomainSocketServer *server;

  bool result = impl.GetInterfaceObject(kINTERFACE_TYPE_SERVER, reinterpret_cast<UnixDomainSocketInterface **>(&server));
  server->Initialize();
//
//  sleep(2);
//  printf ("FinalCAL!!!!!!!!!!");
//  server->Finalize();
  for (;;)
  {
    ;
  }
  std::cout << "Lead Fin";
}
