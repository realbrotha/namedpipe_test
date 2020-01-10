//
// Created by realbro on 1/8/20.
//

#ifndef TESTIPC_UNIXDOMAINSOCKET_INCLUDE_SOCKETWRAPPER_H_
#define TESTIPC_UNIXDOMAINSOCKET_INCLUDE_SOCKETWRAPPER_H_

class SocketWrapper {
 public:
  static bool Create(int &socket_fd, bool non_block_mode = false);
  static bool Connect(int &socket_fd, struct sockaddr_un &sock_addr);
  static bool Bind(int &socket_fd, struct sockaddr_un &sock_addr);
  static bool Listen(int &socket_fd, int backlog_size);
  static bool Accept(int &out_accept_socket_fd, int &listen_socket_fd, struct sockaddr_un &sock_addr);
};

#endif //TESTIPC_UNIXDOMAINSOCKET_INCLUDE_SOCKETWRAPPER_H_
