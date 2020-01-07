//
// Created by realbro on 1/6/20.
//

#include "UnixDomainSocketClient.h"

#include <iostream>

#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
namespace {
constexpr char kFILE_SERVER[] = "/tmp/test_server";
}
UnixDomainSocketClient::UnixDomainSocketClient() {
}

UnixDomainSocketClient::~UnixDomainSocketClient() {
  Finalize();
}

bool UnixDomainSocketClient::Initialize() {
  client_socket = socket(PF_FILE, SOCK_STREAM, 0);
  if (-1 == client_socket) {
    std::cout << "create socket failed" << std::endl;
    return false;
  }

  struct sockaddr_un server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strcpy(server_addr.sun_path, kFILE_SERVER);

  if (-1 == connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
    std::cout << "connect failed" << std::endl;
    return false;
  }
  return true;
}

bool UnixDomainSocketClient::Finalize() {
  close(client_socket);
}

bool UnixDomainSocketClient::SendMessage(std::string &send_string) {
  bool result = false;
  if (client_socket) {
    std::cout << "send string : " << send_string << std::endl;
    write(client_socket, send_string.c_str(), send_string.length() + 1);
    result = true;
  }
  return result;
}