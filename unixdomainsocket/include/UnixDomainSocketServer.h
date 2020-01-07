//
// Created by realbro on 1/6/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_

#include "UnixDomainSocketFactory.h"

#include <atomic>
#include <pthread.h>

class UnixDomainSocketServer : public UnixDomainSocketFactory {
 public :
  UnixDomainSocketServer();
  virtual ~UnixDomainSocketServer();

  virtual bool Initialize();
  virtual bool Finalize();

  static void *EpollHandler(void *arg);

 private :
  int epoll_fd_ = 0;
  int connection_checker_fd = 0;
  pthread_t check_thread = 0;
  std::atomic<bool> stopped_;
};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_
