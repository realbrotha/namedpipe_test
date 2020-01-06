//
// Created by realbro on 1/6/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_

#include "UnixDomainSocketFactory.h"


class UnixDomainSocketServer : public UnixDomainSocketFactory {
 public :
  UnixDomainSocketServer();
 virtual ~UnixDomainSocketServer();

 virtual bool Initialize();
 virtual bool Finalize();
};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_
