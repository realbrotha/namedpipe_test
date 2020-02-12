#include <iostream>
#include <stdio.h>

#include "AsioInterface.h"

int main(int argc, char *argv[]) {
  printf("Asio Server Main!!!!!!~~\n");
  AsioInterface asio_interface;
  AsioServer *obj = nullptr;

  bool result = asio_interface.GetInterfaceObject( INTERFACE_TYPE::SERVER, reinterpret_cast<AsioInterface **>(&obj));
  if (result) {
    obj->Initialize();
    printf("init finsih\n");
    while (1) {
      printf("sleep 111\n");
      sleep(1);
    }

  }

  return 0;
}