//
// Created by realbro on 1/6/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETCLIENT_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETCLIENT_H_

#include "Message.h"

#include <string>
#include <atomic>
#include <list>

#include <sys/socket.h>
#include <sys/un.h>

namespace {
typedef void (*t_ListenerCallbackProc)(Message& msg);
}

class UnixDomainSocketClient {
 public :
  UnixDomainSocketClient();
  virtual ~UnixDomainSocketClient();

  virtual bool Initialize(int& product_code, t_ListenerCallbackProc callback);
  virtual bool Finalize();
  virtual bool SendMessage(std::string &send_string);
  virtual bool SendMessage(int msg_id, std::string &send_string);
  virtual std::string SendMessage(std::string &send_string, int wait_time/* msec , 1000, 2000 ... */);

 private :
  bool WriteMessage(const int write_fd, const void* data_, const ssize_t data_size);
  static void *MainHandler(void *arg);

  int product_code_;
  int epoll_fd_;
  int client_socket_fd_;

  struct sockaddr_un server_addr_ = {0, {0,}};

  std::atomic<bool> isEpollAdded_;
  std::atomic<bool> isConnected_;
  std::atomic<bool> stopped_;

  MessageManager message_manager_;
  int message_id;
  t_ListenerCallbackProc async_message_callback_;
  std::map<int, bool> response_msg_id;
  std::map<int, Message /* msg id, Message*/> need_response_message_;
};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETCLIENT_H_
