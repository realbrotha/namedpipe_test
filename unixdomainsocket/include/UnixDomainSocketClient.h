//
// Created by realbro on 1/6/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETCLIENT_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETCLIENT_H_

#include <string>
#include <atomic>

#include <sys/socket.h>
#include <sys/un.h>

class UnixDomainSocketClient {
 public :
  UnixDomainSocketClient();
  virtual ~UnixDomainSocketClient();

  virtual bool Initialize();
  virtual bool Finalize();
  virtual bool SendMessage(std::string &send_string);

 private :
  static void *MainHandler(void *arg);

  int epoll_fd_ = -1;
  int client_socket_fd_ = -1;
  pthread_t epoll_thread_ = -1;
  struct sockaddr_un server_addr_ = {0, {0,}};

  std::atomic<bool> isEpollAdded_;
  std::atomic<bool> isConnected_;
  std::atomic<bool> stopped_;
};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETCLIENT_H_
