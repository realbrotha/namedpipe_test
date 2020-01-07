//
// Created by realbro on 1/6/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETINTERFACE_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETINTERFACE_H_

#include "UnixDomainSocketFactory.h"
#include "UnixDomainSocketServer.h"
#include "UnixDomainSocketClient.h"

#include <string>

namespace {
constexpr unsigned int kINTERFACE_TYPE_SERVER = 1;
constexpr unsigned int kINTERFACE_TYPE_CLIENT = 2;
}

class UnixDomainSocketInterface : public UnixDomainSocketFactory {
 public:
  UnixDomainSocketInterface();
  virtual ~UnixDomainSocketInterface();

  bool GetInterfaceObject(unsigned int type, UnixDomainSocketInterface **out_interface_ptr);

};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETINTERFACE_H_
