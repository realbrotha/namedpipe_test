//
// Created by realbro on 1/6/20.
//

#include "UnixDomainSocketServer.h"
#include "FileDescriptorTool.h"

#include <iostream>

#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <sys/stat.h>
#include <sys/epoll.h>

#define  FILE_SERVER "/tmp/test_server"

UnixDomainSocketServer::UnixDomainSocketServer() {
}

UnixDomainSocketServer::~UnixDomainSocketServer() {
}

bool UnixDomainSocketServer::Initialize() {
}

bool UnixDomainSocketServer::Finalize() {
}

bool UnixDomainSocketServer::EpollHandler() {
  if (0 == access(FILE_SERVER, F_OK))
    unlink(FILE_SERVER);

  int connection_checker_fd = 0;
  connection_checker_fd = socket(PF_FILE, SOCK_STREAM, 0);

  if (-1 == connection_checker_fd) {
    printf("create socket failed\n");
  }
  struct sockaddr_un server_addr;

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strcpy(server_addr.sun_path, FILE_SERVER);

  if (-1 == bind(connection_checker_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
    printf("bind failed\n");
  }

  if (-1 == listen(connection_checker_fd, 5 /*back log size ??? 정확히 알아 볼것 */)) {
    printf("listen failed\n");
  }

  int epoll_fd = epoll_create(1);
  if (epoll_fd < 0) {
    std::cout << "epoll create failed";
    return false;
  }
  FileDescriptorTool::SetCloseOnExec(epoll_fd, true);
  FileDescriptorTool::SetNonBlock(epoll_fd, true);

  //epollFd_ = epollFd;

  epoll_event settingEvent = {0, {0}};
  settingEvent.data.fd = connection_checker_fd;
  settingEvent.events = EPOLLIN;

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_checker_fd, &settingEvent)) {
    std::cout << "NamedPipeManager::HandleMain: epoll_ctl" << std::endl;
    return false;
  }
}