//
// Created by realbro on 1/6/20.
//

#include "UnixDomainSocketInterface.h"
#include "UnixDomainSocketServer.h"
#include "UnixDomainSocketClient.h"

namespace {
enum ACTIVATE_TYPE {
  SERVER = 30000,
  CLIENT = 40000
};
}

UnixDomainSocketInterface::UnixDomainSocketInterface() {
}

UnixDomainSocketInterface::~UnixDomainSocketInterface() {
}

bool UnixDomainSocketInterface::GetInterfaceObject(unsigned int type, UnixDomainSocketInterface **out_interface_ptr) {
  printf("Get obj \n");
  UnixDomainSocketInterface *interface_object = nullptr;

  bool result = false;
  switch (type) {
    case ACTIVATE_TYPE::SERVER : {
      UnixDomainSocketServer *server_object = nullptr;
      server_object = new UnixDomainSocketServer;
      if (server_object) {
        interface_object = reinterpret_cast<UnixDomainSocketInterface *>(server_object);
        result = true;
      }
    }
    case ACTIVATE_TYPE::CLIENT :
      // do something

    default:break;
  }

  return result;
}