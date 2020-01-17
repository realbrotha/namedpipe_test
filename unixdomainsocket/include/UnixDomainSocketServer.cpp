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

UnixDomainSocketServer::UnixDomainSocketServer() : epoll_fd_(-1),
                                                   accept_checker_fd_(-1),
                                                   server_addr_({0,}){
}

UnixDomainSocketServer::~UnixDomainSocketServer() {
  Finalize();
}

bool UnixDomainSocketServer::Initialize(t_ListenerCallbackProc callback) {
  if (0 == access(kFILE_NAME, F_OK))
    unlink(kFILE_NAME);

  if (!SocketWrapper::Create(accept_checker_fd_)) {
    std::cout << "Socket Create Failed" << std::endl;
    return false;
  }
  async_message_callback_ = callback;

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
          Message msg(reinterpret_cast<const uint8_t *>(message), read_size); // 메세지 제작
          char read_more[kMAX_READ_SIZE] = {0,};
          int read_more_size = 0;
          int total_read_size = read_size;
          do  // read more
          {
            memset(read_more, 0x00, sizeof(read_more));
            read_more_size = read(client_fd, read_more, sizeof(read_more));
            if (read_more_size > 0) {
              msg.AppendData(read_more, read_more_size); // 메세지 제작
              total_read_size += read_more_size;
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

            int msgid = msg_parser.GetMessageId();
            for (auto &it : mgr->response_msg_id) {   // response 를 기다려야 하는 메세지들
              if (it.first == msgid) {
                printf("match case!!!");
                mgr->need_response_message_[msgid] = msg_complete;
                break;
              }
            }
            if (mgr->async_message_callback_) {
              printf("*****callback function called*****\n");
              mgr->async_message_callback_(msg_complete);
            }
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

bool UnixDomainSocketServer::WriteMessage(const int write_fd, const void *data_, const ssize_t data_size) {
  bool result = true;
  int next_post = 0;
  while (next_post < data_size) {
    ssize_t write_result_size = write(write_fd, data_ + next_post, data_size - next_post);
    if (0 > write_result_size) {
      usleep(100);
      continue;
    } else { // 처리
      next_post += write_result_size;
    }
  }
  return result;
}
bool UnixDomainSocketServer::SendMessageBroadcast(std::string &send_string) {
  bool result = false;
  std::map<int, int> buffer;;
  if (UnixDomainSocketSessionManager::GetInstance().GetSocketFdAll(buffer)) {
    Message send_msg(send_string.c_str(), send_string.length(), 1, ++message_id);
    std::cout << " send Length : " << send_msg.GetRawData().size() << std::endl;
    for (auto it = buffer.begin(); it != buffer.end(); it++) {
      ssize_t data_size = send_msg.GetRawData().size();
      if (WriteMessage(it->second, &send_msg.GetRawData()[0], data_size)) result = true;
    }
  }
  printf("Server SendMessage Complete\n");
  return result;
}

bool UnixDomainSocketServer::SendMessage(std::string &send_string, int &product_code) {
  int socket_fd = 0;
  bool result = false;
  if (UnixDomainSocketSessionManager::GetInstance().GetSocketFd(product_code, socket_fd)) {
    Message send_msg(send_string.c_str(), send_string.length(), 1, ++message_id);
    ssize_t data_size = send_msg.GetRawData().size();
    std::cout << " send Length : " << send_msg.GetRawData().size() << std::endl;
    if (WriteMessage(socket_fd, &send_msg.GetRawData()[0], data_size)) result = true;
  }
  return result;
}

bool UnixDomainSocketServer::SendMessage(int msg_id, std::string &send_string, int & product_code) {
  int socket_fd = 0;
  bool result = false;
  if (UnixDomainSocketSessionManager::GetInstance().GetSocketFd(product_code, socket_fd)) {
    Message send_msg(send_string.c_str(), send_string.length(), 1, msg_id);
    ssize_t data_size = send_msg.GetRawData().size();
    std::cout << " send Length : " << send_msg.GetRawData().size() << std::endl;
    if (WriteMessage(socket_fd, &send_msg.GetRawData()[0], data_size)) result = true;
  }
  return result;
}

std::string UnixDomainSocketServer::SendMessage(std::string &send_string, int product_code, int wait_time) {
  std::string result;
  int socket_fd = 0;
  if (UnixDomainSocketSessionManager::GetInstance().GetSocketFd(product_code, socket_fd)) {
    Message send_msg(send_string.c_str(), send_string.length(), 1, ++message_id);
    ssize_t data_size = send_msg.GetRawData().size();
    response_msg_id[message_id] = true;
    if (WriteMessage(socket_fd, &send_msg.GetRawData()[0], data_size)) {
      while (wait_time > 0) {
        if (need_response_message_.count(message_id)) {
          MessageParser msg_parser(need_response_message_[message_id]);
          int data_size = msg_parser.GetDataSize();
          const char *start_pos = reinterpret_cast<const char *>(msg_parser.GetData());
          result.assign(start_pos, data_size);
          break;
        } else {
          usleep(1000);
          wait_time--;
        }
      }
      response_msg_id.erase(message_id);
    }
  }
  return result;
}
