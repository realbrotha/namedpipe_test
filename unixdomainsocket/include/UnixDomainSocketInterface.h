//
// Created by realbro on 1/6/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETINTERFACE_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETINTERFACE_H_

#include "UnixDomainSocketFactory.h"
#include <string>

class UnixDomainSocketInterface : public UnixDomainSocketFactory {
 public:
  UnixDomainSocketInterface();
  virtual ~UnixDomainSocketInterface();

  bool GetInterfaceObject(unsigned int type, UnixDomainSocketInterface **out_interface_ptr);

};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETINTERFACE_H_
