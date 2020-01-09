//
// Created by realbro on 1/7/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSESSIONMANAGER_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSESSIONMANAGER_H_

#include "mutex.hpp"

#include <map>

class UnixDomainSocketSessionManager {
  // TODO : 사실 서버프로그램이 아니라 IPC 용이므로 이러한 메니저가 존재할 이유가 없다. 아무이유 없이 만들었음.
 public :
  static UnixDomainSocketSessionManager &GetInstance();
  ~UnixDomainSocketSessionManager();

  bool Add(int &socket_fd, struct sockaddr_un &addr);
  std::map<int, struct sockaddr_un> GetAll();
  bool Remove(int &socket_fd);
  bool RemoveAll();

 private:
  mutex mutex_;

  std::map<int, struct sockaddr_un> alive_client_session;
};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSESSIONMANAGER_H_
