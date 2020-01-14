//
// Created by realbro on 1/8/20.
//

#ifndef TESTIPC_UNIXDOMAINSOCKET_INCLUDE_EPOLLWRAPPER_H_
#define TESTIPC_UNIXDOMAINSOCKET_INCLUDE_EPOLLWRAPPER_H_

#include <sys/epoll.h>

class EpollWrapper {
 public:
  static bool EpollCreate(int epoll_size, bool blocking_mode, int &out_epoll_fd);
  static bool EpollControll(int &epoll_fd, int target_fd, uint32_t epoll_event_type, int mode, void* data_struct = nullptr);
};

#endif //TESTIPC_UNIXDOMAINSOCKET_INCLUDE_EPOLLWRAPPER_H_
