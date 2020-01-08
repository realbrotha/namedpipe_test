//
// Created by realbro on 1/6/20.
//

#include "UnixDomainSocketClient.h"
#include "FileDescriptorTool.h"
#include "EpollWrapper.h"
#include "SocketWrapper.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <string.h>
#include <unistd.h>

#include <sys/epoll.h>

namespace {
constexpr char kFILE_NAME[] = "/tmp/test_server";
constexpr int kMAX_EVENT_COUNT = 3;
}

UnixDomainSocketClient::UnixDomainSocketClient() {
}

UnixDomainSocketClient::~UnixDomainSocketClient() {
  Finalize();
}

bool UnixDomainSocketClient::Initialize() {
  memset(&server_addr_, 0, sizeof(server_addr_));
  server_addr_.sun_family = AF_UNIX;
  strcpy(server_addr_.sun_path, kFILE_NAME);

  if (!SocketWrapper::Create(client_socket_fd_)) {
    std::cout << "Socket Create Failed" << std::endl;
    return false;
  }
  if (!EpollWrapper::EpollCreate(1, true, epoll_fd_)) {
    std::cout << "Epoll Create Failed" << std::endl;
    return false;
  }

  std::thread main_thread(&UnixDomainSocketClient::MainHandler, this);
  main_thread.detach();

  return true;
}

bool UnixDomainSocketClient::Finalize() {
  stopped_ = true;
  //Thread 종료부분이 Graceful 하지 아니함.
  close(client_socket_fd_);
  close(epoll_fd_);
}

void *UnixDomainSocketClient::MainHandler(void *arg) {
  std::cout << "Client EpollHandler Thread!!!!" << std::endl;
  UnixDomainSocketClient *mgr = reinterpret_cast<UnixDomainSocketClient *>(arg);
  mgr->stopped_ = false;
  mgr->isConnected_ = false;
  mgr->isEpollAdded_ = false;

  while (!mgr->stopped_) {
    bool restart_flag = false;
    if (0 > mgr->client_socket_fd_) { // socket을 생성하고 connection 시킴
      if (!SocketWrapper::Create(mgr->client_socket_fd_)) {
        mgr->isConnected_ = true;
        std::cout << "Socket Connect OK" << std::endl;
      } else {
        std::cout << "Socket Create Failed" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        continue;
      }
    }
    if (!mgr->isConnected_) { // socket을 생성하고 connection 시킴
      if ( SocketWrapper::Connect(mgr->client_socket_fd_, mgr->server_addr_)) {
        mgr->isConnected_ = true;
        std::cout << "Socket Connect OK" << std::endl;
      } else {
        std::cout << "Socket Connect Failed" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        continue;
      }
    }
    if (!mgr->isEpollAdded_) { // Epoll 등록
      std::cout << "EPOLL VALUE : " << mgr->epoll_fd_ << "Remake!!" << std::endl;
      if (EpollWrapper::EpollControll(mgr->epoll_fd_,
                                      mgr->client_socket_fd_,
                                      EPOLLIN | EPOLLOUT | EPOLLERR,
                                      EPOLL_CTL_ADD)) {
        std::cout << "~~~~~epoll controll success~~~~~~~" << std::endl;
        mgr->isEpollAdded_ = true;
      } else {
        std::cout << "~~~~~epoll failed!!!!!!!!!!!!!!!!!!~~~~~~~" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        continue;
      }
    }
    //if (mgr->isEpollAdded_) {}
    // 정상적으로 처리 될경우
    epoll_event gettingEvent[kMAX_EVENT_COUNT] = {0, {0}};
    int event_count = epoll_wait(mgr->epoll_fd_, gettingEvent, kMAX_EVENT_COUNT, 10 /* ms */);

    if (event_count > 0) { // 이벤트 발생, event_count = 0 처리안함.
      std::cout << "EpollHandler::HandleMain: epoll count_: " << event_count << std::endl;
      for (int i = 0; i < event_count; ++i) {
        printf("index [%d], event type [%d], from [%d]\n", i, gettingEvent[i].events, gettingEvent[i].data.fd);
        if (gettingEvent[i].data.fd == mgr->client_socket_fd_)    // 듣기 소켓에서 이벤트가 발생함
        {
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
    if (event_count < 0 || restart_flag) {  // epoll 상에서 문제가 발생할 경우 혹은 쓰레드 재기동이 필요한경우. 조건문이 좀 이상함 바로 위 조건문이랑 맞출것
      std::cout << "***********Error BREAK Restart!!!!" << std::endl;
      EpollWrapper::EpollControll(mgr->epoll_fd_,
                                  mgr->client_socket_fd_,
                                  EPOLLIN | EPOLLOUT | EPOLLERR,
                                  EPOLL_CTL_DEL);
      close(mgr->client_socket_fd_);
      //close(mgr->epoll_fd_);

      mgr->client_socket_fd_ = -1;
      //mgr->epoll_fd_ = -1;
      mgr->isEpollAdded_ = false;
      mgr->isConnected_ = false;
      continue;
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