//
// Created by realbro on 1/6/20.
//

#include "UnixDomainSocketSessionManager.h"
#include "UnixDomainSocketServer.h"
#include "FileDescriptorTool.h"

#include <iostream>

#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>

namespace {
constexpr char kFILE_SERVER[] = "/tmp/test_server";
constexpr int kMAX_EVENT_COUNT = 3;
}
UnixDomainSocketServer::UnixDomainSocketServer() {
}

UnixDomainSocketServer::~UnixDomainSocketServer() {
  Finalize();
}

bool UnixDomainSocketServer::Initialize() {
  std::cout << "Server Side Init";

  if (0 == access(kFILE_SERVER, F_OK))
    unlink(kFILE_SERVER);

  connection_checker_fd = socket(PF_FILE, SOCK_STREAM, 0);

  if (-1 == connection_checker_fd) {
    printf("create socket failed\n");
  }
  struct sockaddr_un server_addr;

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strcpy(server_addr.sun_path, kFILE_SERVER);

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

  epoll_fd_ = epoll_fd;

  epoll_event settingEvent = {0, {0}};
  settingEvent.data.fd = connection_checker_fd;
  settingEvent.events = EPOLLIN | EPOLLOUT | EPOLLERR;

  if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_checker_fd, &settingEvent)) {
    std::cout << "NamedPipeManager::HandleMain: epoll_ctl failed" << std::endl;
    return false;
  }
  pthread_create(&check_thread, NULL, EpollHandler, this);
}

bool UnixDomainSocketServer::Finalize() {
  printf("Finalilze!!!!\n");
  stopped_ = true;
  pthread_join(check_thread, NULL);
}

void *UnixDomainSocketServer::EpollHandler(void *arg) {
  std::cout << "EpollHandler Thread!!!!" << std::endl;
  UnixDomainSocketServer *mgr = reinterpret_cast<UnixDomainSocketServer *>(arg);
  mgr->stopped_ = false;

  while (!mgr->stopped_) {
    epoll_event gettingEvent[kMAX_EVENT_COUNT] = {0, {0}};
    int event_count = epoll_wait(mgr->epoll_fd_, gettingEvent, kMAX_EVENT_COUNT, 10 /* ms */);
    int errorCount = 0;

    if (event_count < 0) // error
    {
      if (errno == EINTR) {
        continue;
      }
      if (++errorCount < 3) {
        std::cout << "Epoll Error++" << std::endl;
        continue;
      }
      break;
    } else if (event_count > 0) // 이벤트 떨어짐.  event_count = 0 처리안함.
    {
      std::cout << "EpollHandler::HandleMain: epoll count_: " << event_count << std::endl;
      for (int i = 0; i < event_count; ++i) {
        printf("index [%d], event type [%d], from [%d]\n", i, gettingEvent[i].events, gettingEvent[i].data.fd);
        if (gettingEvent[i].data.fd == mgr->connection_checker_fd)    // 듣기 소켓에서 이벤트가 발생함
        {
          struct sockaddr_un client_addr;
          int client_socket = 0;
          socklen_t client_size = sizeof(client_addr);

          client_socket = accept(mgr->connection_checker_fd, reinterpret_cast<struct sockaddr *>(&client_addr), &client_size);
          if (client_socket > 0) {
            std::cout << "Socket Accept Called" << std::endl;
            UnixDomainSocketSessionManager::GetInstance().Add(client_socket, client_addr);

            struct epoll_event client_event;
            client_event.events = EPOLLIN;
            client_event.data.fd = client_socket;;
            if (-1 == epoll_ctl(mgr->epoll_fd_, EPOLL_CTL_ADD, client_socket, &client_event)) {
              std::cout << "NamedPipeManager::HandleMain: epoll_ctl" << std::endl;
              continue;
            }
          }
        } else  // accept 이후
        {
          int client_socket_fd = gettingEvent[i].data.fd;

          char message[1024] = {0,};
          int read_size = read(client_socket_fd, message, sizeof(message));

          struct epoll_event client_event;
          client_event.events = EPOLLIN;
          client_event.data.fd = client_socket_fd;

          if (read_size < 0 || read_size == 0) { // Error or Disconnect
            std::cout << "User Disconnect" << std::endl;
            epoll_ctl(mgr->epoll_fd_, EPOLL_CTL_DEL, client_socket_fd, &client_event);
            UnixDomainSocketSessionManager::GetInstance().Remove(client_socket_fd);
            continue;
          }
          std::string buff(message);
          std::cout << "read Message : " << message << std::endl;
        }
      }
    }
  }
  std::cout << "EpollHandler Thread!!!! END OF Thread" << std::endl;
  pthread_exit(NULL);
}