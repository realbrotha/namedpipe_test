//
// Created by realbro on 1/20/20.
//

#include "AsioInterface.h"

AsioInterface::AsioInterface() {

}
AsioInterface::~AsioInterface() {

}
bool AsioInterface::GetInterfaceObject(unsigned int type, AsioInterface **out_interface_ptr) {

  bool result = false;
  AsioInterface *interface_object = nullptr;

  switch (type) {
    case INTERFACE_TYPE::CLIENT : {
      AsioClient *client_object = nullptr;
      client_object = new AsioClient;
      if (client_object) {
        interface_object = reinterpret_cast<AsioInterface *>(client_object);
        result = true;
      }
      break;
    }
    case INTERFACE_TYPE::SERVER : {
      AsioServer *server_object = nullptr;
      server_object = new AsioServer;

      if (server_object) {
        interface_object = reinterpret_cast<AsioInterface *>(server_object);
        result = true;
      }
      break;
    }
  }
  *out_interface_ptr = interface_object;
  return result;
}
