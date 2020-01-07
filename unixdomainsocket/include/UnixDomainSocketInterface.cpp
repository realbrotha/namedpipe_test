//
// Created by realbro on 1/6/20.
//

#include "UnixDomainSocketInterface.h"

#include <iostream>

UnixDomainSocketInterface::UnixDomainSocketInterface() {
}

UnixDomainSocketInterface::~UnixDomainSocketInterface() {
}

// TODO : Extern 으로 외부에 노출시키도록 변경할것 ( header --> so)
bool UnixDomainSocketInterface::GetInterfaceObject(unsigned int type, UnixDomainSocketInterface **out_interface_ptr) {
  UnixDomainSocketInterface *interface_object = nullptr;
  bool result = false;

  switch (type) {
    case kINTERFACE_TYPE_SERVER : {
      UnixDomainSocketServer *server_object = nullptr;
      server_object = new UnixDomainSocketServer;
      if (server_object) {
        interface_object = reinterpret_cast<UnixDomainSocketInterface *>(server_object);
        result = true;
      }
      break;
    }
    case kINTERFACE_TYPE_CLIENT : {
      // do something
      UnixDomainSocketClient *client_object = nullptr;
      client_object = new UnixDomainSocketClient;
      if (client_object) {
        std::cout << " clien get";
        interface_object = reinterpret_cast<UnixDomainSocketInterface *>(client_object);
        result = true;
      }
      break;
    }
  }
  *out_interface_ptr = interface_object;
  return result;
}