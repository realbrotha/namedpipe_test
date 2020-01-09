//
// Created by realbro on 1/8/20.
//

#include "SocketWrapper.h"

#include <sys/un.h>
#include <sys/socket.h>
#include <iostream>

bool SocketWrapper::Create(int &socket_fd) {
  socket_fd = socket(PF_FILE, SOCK_STREAM, 0);
  if (-1 == socket_fd) {
    std::cout << "create socket failed" << std::endl;
    return false;
  }
  std::cout << "create socket success" << std::endl;
  return true;
}

bool SocketWrapper::Connect(int &socket_fd, struct sockaddr_un &sock_addr) {
  return (-1 == connect(socket_fd, reinterpret_cast<struct sockaddr *>(&sock_addr), sizeof(sock_addr))) ? false : true;
}

bool SocketWrapper::Bind(int &socket_fd, struct sockaddr_un &sock_addr) {
  return (-1 == bind(socket_fd, reinterpret_cast<struct sockaddr *>(&sock_addr), sizeof(sock_addr))) ? false : true;
}

bool SocketWrapper::Listen(int &socket_fd, int backlog_size) {
  return (-1 == listen(socket_fd, backlog_size)) ? false : true;
}

bool SocketWrapper::Accept(int &out_accept_socket_fd, int &listen_socket_fd, struct sockaddr_un &sock_addr) {
  socklen_t client_socket_size = sizeof(sock_addr);
  out_accept_socket_fd = accept(listen_socket_fd, reinterpret_cast<struct sockaddr *>(&sock_addr), &client_socket_size);
  return (-1 == out_accept_socket_fd) ? false : true;
}
