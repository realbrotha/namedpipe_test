//
// Created by realbro on 1/2/20.
//

#ifndef EPOLL_EPOLLHANDLER_H
#define EPOLL_EPOLLHANDLER_H

#include <atomic>

#define AYN_DISALLOW_COPY_AND_ASSIGN(TypeName)   \
private:                                         \
    TypeName(const TypeName&);                   \
    TypeName& operator=(const TypeName&);        \
public:

class EpollHandler {
 public:
  EpollHandler();
  ~EpollHandler();

 AYN_DISALLOW_COPY_AND_ASSIGN(EpollHandler);

  bool Initialize(int fd);
  void Uninitialize();

  void Stop();
  void HandlerMain();

 private:

  int notifyFd_;
  int epollFd_;

  std::atomic<bool> stopped_;
};

#endif //EPOLL_EPOLLHANDLER_H
