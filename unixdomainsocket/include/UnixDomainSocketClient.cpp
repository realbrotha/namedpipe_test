//
// Created by realbro on 1/6/20.
//

#include "UnixDomainSocketClient.h"
#include "FileDescriptorTool.h"
#include "EpollWrapper.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/epoll.h>

namespace {
constexpr char kFILE_SERVER[] = "/tmp/test_server";
constexpr int kMAX_EVENT_COUNT = 3;
}

UnixDomainSocketClient::UnixDomainSocketClient() {
}

UnixDomainSocketClient::~UnixDomainSocketClient() {
  Finalize();
}

bool UnixDomainSocketClient::Initialize() {
  return SocketRun(client_socket_fd_, server_addr_);
}

bool UnixDomainSocketClient::Finalize() {
  stopped_ = true;
  pthread_join(epoll_thread_, NULL);
  pthread_join(socket_thread_, NULL);

  close(client_socket_fd_);
  close(epoll_fd_);
}

bool UnixDomainSocketClient::SocketRun(int &socket_fd, struct sockaddr_un &sock_addr) {
  if (socket_fd) {
    close(socket_fd);
    socket_fd = -1;
  }

  socket_fd = socket(PF_FILE, SOCK_STREAM, 0);
  if (-1 == socket_fd) {
    std::cout << "create socket failed" << std::endl;
    return false;
  }
  memset(&sock_addr, 0, sizeof(sock_addr));
  sock_addr.sun_family = AF_UNIX;
  strcpy(sock_addr.sun_path, kFILE_SERVER);

  pthread_create(&socket_thread_, NULL, SocketHandler, this);
  return true;
}

void UnixDomainSocketClient::EpollRun(int socket_fd) {
  if (epoll_fd_) {
    close(epoll_fd_);
    epoll_fd_ = -1;
  }
  if (EpollWrapper::EpollCreate(1, true, epoll_fd_) &&
      EpollWrapper::EpollControll(epoll_fd_, client_socket_fd_, EPOLLIN | EPOLLOUT | EPOLLERR, EPOLL_CTL_ADD)) {
    std::cout << "~~~~~epoll controll start~~~~~~~" << std::endl;

    pthread_create(&epoll_thread_, NULL, EpollHandler, this);
  }
}

void *UnixDomainSocketClient::SocketHandler(void *arg) {
  UnixDomainSocketClient *mgr = reinterpret_cast<UnixDomainSocketClient *>(arg);
  mgr->stopped_ = false;
  int error_count = 0;
  while (!mgr->stopped_) {
    if (-1 != connect(mgr->client_socket_fd_, (struct sockaddr *) &mgr->server_addr_, sizeof(server_addr_))) {
      std::cout << "connect success" << std::endl;
      mgr->EpollRun(mgr->client_socket_fd_);
      break;
    } else {
      std::cout << "cannot connect" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  pthread_exit(NULL);
}

void *UnixDomainSocketClient::EpollHandler(void *arg) {
  std::cout << "Client EpollHandler Thread!!!!" << std::endl;
  UnixDomainSocketClient *mgr = reinterpret_cast<UnixDomainSocketClient *>(arg);
  mgr->stopped_ = false;
  bool restart_flag = false;

  while (!mgr->stopped_) {
    if (-1 == mgr->epoll_fd_) break;
    epoll_event gettingEvent[kMAX_EVENT_COUNT] = {0, {0}};
    int event_count = epoll_wait(mgr->epoll_fd_, gettingEvent, kMAX_EVENT_COUNT, 10 /* ms */);
    if (event_count < 0 || restart_flag) {  // epoll 상에서 문제가 발생할 경우 혹은 쓰레드 재기동이 필요한경우.
      std::cout << "Error BREAK Restart!!!!" << std::endl;
      mgr->SocketRun(mgr->client_socket_fd_, mgr->server_addr_);
      break;
    } else if (event_count > 0) { // 이벤트 발생, event_count = 0 처리안함.
      std::cout << "EpollHandler::HandleMain: epoll count_: " << event_count << std::endl;
      for (int i = 0; i < event_count; ++i) {
        printf("index [%d], event type [%d], from [%d]\n", i, gettingEvent[i].events, gettingEvent[i].data.fd);
        if (gettingEvent[i].data.fd == mgr->client_socket_fd_)    // 듣기 소켓에서 이벤트가 발생함
        {
          /*
          struct sockaddr_un client_addr;
          memset(&client_addr, 0x00, sizeof(sockaddr_un));
          socklen_t client_size = sizeof(client_addr);
          */
          char message[1024] = {0,};
          int read_size = read(mgr->client_socket_fd_, message, sizeof(message));

          if (read_size < 0 || read_size == 0) { // Error or Disconnect
            std::cout << "Disconnect Event !!!!!!!!!" << std::endl;
            restart_flag = true;
            break;
          }
          std::string buff(message);
          std::cout << "client read Message : " << message << std::endl;
        }
      }
    }
  }
  std::cout << "Client EpollHandler Thread!!!! END OF Thread" << std::endl;
  pthread_exit(NULL);
}

bool UnixDomainSocketClient::SendMessage(std::string &send_string) {
  bool result = false;
  if (0 < client_socket_fd_) {
    std::cout << "send string : " << send_string << std::endl;
    write(client_socket_fd_, send_string.c_str(), send_string.length() + 1);
    result = true;
  }
  return result;
}