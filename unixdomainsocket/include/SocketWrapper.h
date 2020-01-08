//
// Created by realbro on 1/8/20.
//

#ifndef TESTIPC_UNIXDOMAINSOCKET_INCLUDE_SOCKETWRAPPER_H_
#define TESTIPC_UNIXDOMAINSOCKET_INCLUDE_SOCKETWRAPPER_H_

class SocketWrapper {
 public:
  static bool Create(int &socket_fd);
  static bool Connect(int &socket_fd, struct sockaddr_un &sock_addr);
};

#endif //TESTIPC_UNIXDOMAINSOCKET_INCLUDE_SOCKETWRAPPER_H_
