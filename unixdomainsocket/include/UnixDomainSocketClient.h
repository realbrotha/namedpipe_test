//
// Created by realbro on 1/6/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETCLIENT_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETCLIENT_H_

#include <string>
#include <atomic>

#include <sys/un.h>

class UnixDomainSocketClient {
 public :
  UnixDomainSocketClient();
  virtual ~UnixDomainSocketClient();

  virtual bool Initialize();
  virtual bool Finalize();
  virtual bool SendMessage(std::string &send_string);

 private :
  void EpollRun(int socket_fd);
  bool SocketRun(int &socket_fd,  struct sockaddr_un & sock_addr);

  static void *EpollHandler(void *arg);
  static void *SocketHandler(void *arg);

  bool EpollControll(int &in_target_fd, uint32_t &in_epoll_event, int &out_epool_fd);

  int epoll_fd_ = -1;
  int client_socket_fd_ = -1;
  pthread_t epoll_thread_ = -1;
  pthread_t socket_thread_ = -1;
  struct sockaddr_un server_addr_ = {0, {0,}};

  std::atomic<bool> stopped_;
};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETCLIENT_H_
