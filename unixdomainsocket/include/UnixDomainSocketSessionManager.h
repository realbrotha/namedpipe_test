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
  bool Remove(int &socket_fd);

  std::map<int, int> GetAllSession();
  bool RemoveAll();

  MessageManager &GetMessageManager();
 private:
  mutex mutex_;
  MessageManager message_manager_;

  std::map<int, int /*product, socket_fd*/> socket_list_;
};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSESSIONMANAGER_H_
