//
// Created by realbro on 1/6/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_

#include "UnixDomainSocketFactory.h"

#include <string>
#include <atomic>

#include <pthread.h>
#include <sys/un.h>

struct UserEpoll{
  int product_code;
  int target_td;
};

class UnixDomainSocketServer : public UnixDomainSocketFactory {
 public :
  UnixDomainSocketServer();
  virtual ~UnixDomainSocketServer();

  virtual bool Initialize();
  virtual bool Finalize();

  virtual bool SendMessageBroadcast(std::string &send_string);

 private :
  static void *EpollHandler(void *arg);

  int epoll_fd_ = 0;
  int accept_checker_fd_ = 0;

  struct sockaddr_un server_addr_ = {0, {0,}};
  std::atomic<bool> stopped_;

  struct UserEpoll struct_user_epoll_;
};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_
