//
// Created by realbro on 1/6/20.
//

#include "Message.h"
#include "UnixDomainSocketSessionManager.h"
#include "UnixDomainSocketServer.h"
#include "SocketWrapper.h"
#include "EpollWrapper.h"
#include "FileDescriptorTool.h"

#include <iostream>
#include <thread>

#include <string.h>
#include <unistd.h>

#include <sys/socket.h>

namespace {
constexpr char kFILE_NAME[] = "/tmp/test_server";
constexpr char kFIND_STRING[] = "product:";
constexpr int kMAX_EVENT_COUNT = 3;
constexpr int kMAX_READ_SIZE = 2048;
}
UnixDomainSocketServer::UnixDomainSocketServer() {
}

UnixDomainSocketServer::~UnixDomainSocketServer() {
  Finalize();
}

bool UnixDomainSocketServer::Initialize() {
  std::cout << "Server Side Init";

  if (0 == access(kFILE_NAME, F_OK))
    unlink(kFILE_NAME);

  if (!SocketWrapper::Create(accept_checker_fd_)) {
    std::cout << "Socket Create Failed" << std::endl;
    return false;
  }

  memset(&server_addr_, 0, sizeof(server_addr_));
  server_addr_.sun_family = AF_UNIX;
  strcpy(server_addr_.sun_path, kFILE_NAME);

  if (!SocketWrapper::Bind(accept_checker_fd_, server_addr_)) {
    std::cout << "Socket Bind Failed" << std::endl;
    return false;
  }
  if (!SocketWrapper::Listen(accept_checker_fd_, 5)) {
    std::cout << "Socket Listen Failed" << std::endl;
    return false;
  }

  if (!EpollWrapper::EpollCreate(1, true, epoll_fd_)) {
    std::cout << "Epoll Create Failed" << std::endl;
    return false;
  }

  if (!EpollWrapper::EpollControll(epoll_fd_,
                                   accept_checker_fd_,
                                   EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET,
                                   EPOLL_CTL_ADD)) {
    std::cout << "epoll add failed" << std::endl;
    return false;
  }

  std::thread server_socket_thread(&UnixDomainSocketServer::EpollHandler, this);
  server_socket_thread.detach();
  return true;
}

bool UnixDomainSocketServer::Finalize() {
  stopped_ = true;

  close(accept_checker_fd_);
  close(epoll_fd_);
}

#include <sys/ioctl.h>
#include <stdlib.h>

void *UnixDomainSocketServer::EpollHandler(void *arg) {
  std::cout << "EpollHandler Thread!!!!" << std::endl;
  UnixDomainSocketServer *mgr = reinterpret_cast<UnixDomainSocketServer *>(arg);
  mgr->stopped_ = false;

  int unknown_user_count = 0;
  int errorCount = 0;
  int target_size = 0;

  MessageManager message_manager;
  while (!mgr->stopped_) {
    epoll_event gettingEvent[kMAX_EVENT_COUNT] = {0, {0}};
    int event_count = epoll_wait(mgr->epoll_fd_, gettingEvent, kMAX_EVENT_COUNT, 10 /* ms */);

    if (event_count > 0) // 이벤트 떨어짐.  event_count = 0 처리안함.
    {
      for (int i = 0; i < event_count; ++i) {
        printf("epoll event index [%d], event type [%d]\n", i, gettingEvent[i].events);

        if (gettingEvent[i].data.fd == mgr->accept_checker_fd_)    // accept
        {
          struct sockaddr_un client_addr;
          int client_socket = 0;

          if (!SocketWrapper::Accept(client_socket, mgr->accept_checker_fd_, client_addr)) {
            std::cout << "Socket Accept Failed" << std::endl;
            continue;
          } else { //  accept 성공시
            std::cout << "Socket Accept Called" << std::endl;
            FileDescriptorTool::SetNonBlock(client_socket, true);
            if (!EpollWrapper::EpollControll(mgr->epoll_fd_,
                                             client_socket,
                                             EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET,
                                             EPOLL_CTL_ADD)) {
              std::cout << "epoll add failed" << std::endl;
              continue;
            }
          }
        } else { // accept 이후
          int client_fd = gettingEvent[i].data.fd;
          char message[kMAX_READ_SIZE] = {0,};
          memset (message, 0x00, sizeof(message));
          int read_size = read(client_fd, message, sizeof(message));
          if (read_size < 0) {   // read 할게없음
            continue;
          }
          else if( read_size == 0) { // Disconnect
            std::cout << "User Disconnect" << std::endl;
            EpollWrapper::EpollControll(mgr->epoll_fd_,
                                        client_fd,
                                        EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET,
                                        EPOLL_CTL_DEL);
            UnixDomainSocketSessionManager::GetInstance().Remove(client_fd);
            continue;
          } else { // Read 정상.
            printf("Message Recv!!!!!!! read size :%d\n",read_size);
            Message msg(reinterpret_cast<const uint8_t *>(message), read_size); // 메세지 제작

            char read_more[kMAX_READ_SIZE] = {0,};
            int read_more_size = 0;
            do  // read more
            {
              printf ("???????????????\n");
              memset(read_more, 0x00, sizeof(read_more));
              read_more_size = read(client_fd, read_more, sizeof(read_more));
              if (read_more_size > 0) {
                printf ("read more append size : (%d)\n", read_more_size);
                msg.AppendData(read_more, read_more_size); // 메세지 제작
              }
            } while (read_more_size > 0);

            MessageParser parser(msg);

            if (parser.IsHeader()) { // header 정보가 있을경우
              std::cout << "this is header!!!!" << std::endl;
              printf ( "this Message Size : (%d) //data size : (%d)\n", target_size, parser.GetDataSize());
              printf ("recv Data : (%s)\n", parser.GetData());
            } else {  // header 정보가 없을 경우
              printf("!!!!!!!!!read size : %d\n", read_size);
              std::string buff2(message);
              if (buff2.find(kFIND_STRING, 0) != std::string::npos) { // 처음 연결된 후 약속된 데이터 처리 제품 코드를 던져서 식별을 한다.
                int product_type = std::atoi(&buff2.back());
                printf("product AD!!!!!!!!!! code : %d\n", product_type);
                UnixDomainSocketSessionManager::GetInstance().Add(product_type, client_fd);

                EpollWrapper::EpollControll(mgr->epoll_fd_,
                                            client_fd,
                                            EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET,
                                            EPOLL_CTL_MOD);
              } else {
                printf("not header \n");
              }
            }
          }
        }
      }
    } else if (event_count < 0) // epoll error
    {
      if (errno == EINTR) {
        continue;
      }
      if (++errorCount < 3) {
        if (!(EpollWrapper::EpollCreate(1, true, mgr->epoll_fd_) &&
            EpollWrapper::EpollControll(mgr->epoll_fd_,
                                        mgr->accept_checker_fd_,
                                        EPOLLIN | EPOLLOUT | EPOLLERR,
                                        EPOLL_CTL_ADD))) {
          std::cout << "epoll add failed" << std::endl;
          break; // 한번해보고 실패시 종료.
        }
        continue;
      }
      break; // 에러가 많으면 종료
    }
  }
  std::cout << "EpollHandler Thread!!!! END OF Thread" << std::endl;
}

bool UnixDomainSocketServer::SendMessageBroadcast(std::string &send_string) {
  bool result = false;
  std::map<int, int> buffer = UnixDomainSocketSessionManager::GetInstance().GetAllSession();

  if (!buffer.empty()) {
    std::cout << "server send string : " << send_string << std::endl;
    for (auto it = buffer.begin(); it != buffer.end(); it++) {
      write(it->second, send_string.c_str(), send_string.length() + 1);
    }
    result = true;
  }
  return result;
}