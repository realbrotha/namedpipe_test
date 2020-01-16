//
// Created by realbro on 1/7/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSESSIONMANAGER_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSESSIONMANAGER_H_

#include "mutex.hpp"
#include "Message.h"

#include <map>

class UnixDomainSocketSessionManager {
 public :
  static UnixDomainSocketSessionManager &GetInstance();
  ~UnixDomainSocketSessionManager();

  bool Add(int &product_code, int &socket_fd);
  bool Remove(int &product_code);
  bool RemoveAll();

  bool GetProductCode(int &product_code, int &socket_fd);
  bool GetSocketFd(int &product_code, int &socket_fd);
  bool GetSocketFdAll(std::map<int, int>& socket_list);
  int32_t GetSocketFdSize();
 private:
  mutex mutex_;

  std::map<int, int /*socket_fd, product*/> reverse_socket_list_;
  std::map<int, int /*product, socket_fd*/> socket_list_;
};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSESSIONMANAGER_H_

