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
#include <string_view>

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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

void *UnixDomainSocketServer::EpollHandler(void *arg) {
  std::cout << "EpollHandler Thread!!!!" << std::endl;
  UnixDomainSocketServer *mgr = reinterpret_cast<UnixDomainSocketServer *>(arg);
  mgr->stopped_ = false;

  int errorCount = 0;
  MessageManager message_manager;
  while (!mgr->stopped_) {
    epoll_event gettingEvent[kMAX_EVENT_COUNT] = {0, {0}};
    int event_count = epoll_wait(mgr->epoll_fd_, gettingEvent, kMAX_EVENT_COUNT, 10 /* ms */);

    if (event_count > 0) // 이벤트 떨어짐.  event_count = 0 처리안함.
    {
      for (int i = 0; i < event_count; ++i) {
        printf("epoll event index [%d], event type [%d]\n", i, gettingEvent[i].events);

        if (gettingEvent[i].data.fd == mgr->accept_checker_fd_) // accept
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
          memset(message, 0x00, sizeof(message));
          int read_size = read(client_fd, message, sizeof(message));
          if (read_size < 0) {   // read 할게없음
            continue;
          } else if (read_size == 0) { // Disconnect
            std::cout << "User Disconnect" << std::endl;
            EpollWrapper::EpollControll(mgr->epoll_fd_,
                                        client_fd,
                                        EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET,
                                        EPOLL_CTL_DEL);
            UnixDomainSocketSessionManager::GetInstance().Remove(client_fd);
            continue;
          }
          // Read 정상.
          printf("Message Recv!!!!!!! read size :%d\n", read_size);
          Message msg(reinterpret_cast<const uint8_t *>(message), read_size); // 메세지 제작
          char read_more[kMAX_READ_SIZE] = {0,};
          int read_more_size = 0;
          do  // read more
          {
            memset(read_more, 0x00, sizeof(read_more));
            read_more_size = read(client_fd, read_more, sizeof(read_more));
            if (read_more_size > 0) {
              msg.AppendData(read_more, read_more_size); // 메세지 제작
            }
          } while (read_more_size > 0);
          int product = 0;
          if (UnixDomainSocketSessionManager::GetInstance().GetSocketFdSize() &&
              !UnixDomainSocketSessionManager::GetInstance().GetProductCode(product, client_fd)) {
            // product 정보가 없다면 아래를 처리할 이유가 없다.
            continue;
          }

          MessageParser parser(msg);
          if (parser.IsHeader()) { // header 정보가 있을경우
            message_manager.Add(product, msg);
          } else {  // communicated product type
            std::string buff(message);
            if (buff.find(kFIND_STRING, 0) != std::string::npos) { // 처음 연결된 후 약속된 데이터 처리 제품 코드를 던져서 식별을 한다.
              int product_type = std::atoi(&buff.back());
              UnixDomainSocketSessionManager::GetInstance().Add(product_type, client_fd);
            } else { // header -info
              if (message_manager.IsExistMessage(product)) {
                Message msg_need_append = message_manager.MessagePop(product);
                msg_need_append.AppendData(reinterpret_cast<char *>(&msg.GetRawData()[0]), msg.GetMessageSize());
                message_manager.Add(product, msg_need_append);
              }
            }
          }

          if (message_manager.IsPerfectMessage(product)) {
            Message msg_complete = message_manager.MessagePop(product);
            MessageParser msg_parser(msg_complete);
            printf("*****msg size1 : [%d]*****\n", msg_complete.GetMessageSize());
            printf("*****msg size2 : [%d]*****\n", msg_parser.GetDataSize());
            printf("*****Message Done*****\n");
          }
        }
      }
    } else if (event_count < 0) {  // epoll error
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
  std::map<int, int> buffer;;
  if (UnixDomainSocketSessionManager::GetInstance().GetSocketFdAll(buffer)) {
    Message send_msg(send_string.c_str(), send_string.length(), 1, 1);
    std::cout << " send Length : " << send_msg.GetRawData().size() << std::endl;
    for (auto it = buffer.begin(); it != buffer.end(); it++) {
      ssize_t data_size = send_msg.GetRawData().size();
      int next_post = 0;
      while (next_post < data_size) {
        size_t write_result_size = write(it->second, &send_msg.GetRawData()[next_post], data_size - next_post);
        next_post += write_result_size;
        if (0 < write_result_size) {
          usleep(100);
          continue;
        } else if (write_result_size < send_msg.GetRawData().size()) {

        }
      }
    }
    result = true;
  }
  return result;
}