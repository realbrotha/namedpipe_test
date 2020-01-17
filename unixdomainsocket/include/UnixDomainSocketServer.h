//
// Created by realbro on 1/6/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_

#include "UnixDomainSocketFactory.h"
#include "Message.h"

#include <string>
#include <atomic>
#include <list>

#include <pthread.h>
#include <sys/un.h>

namespace {
typedef void (*t_ListenerCallbackProc)(Message& msg);
}

class UnixDomainSocketServer : public UnixDomainSocketFactory {
 public :
  UnixDomainSocketServer();
  virtual ~UnixDomainSocketServer();

  virtual bool Initialize(t_ListenerCallbackProc callback = NULL);
  virtual bool Finalize();

  virtual bool SendMessage(std::string &send_string,int & product_code);
  virtual bool SendMessage(int msg_id, std::string &send_string, int & product_code);
  virtual std::string SendMessage(std::string &send_string, int product_code, int wait_time);

  virtual bool SendMessageBroadcast(std::string &send_string);

 private :
  bool WriteMessage(const int write_fd, const void* data_, const ssize_t data_size);
  static void *EpollHandler(void *arg);

  int epoll_fd_ ;
  int accept_checker_fd_;

  struct sockaddr_un server_addr_ = {0, {0,}};
  std::atomic<bool> stopped_;

  t_ListenerCallbackProc async_message_callback_;
  int message_id = 0;
  std::map<int, bool> response_msg_id;
  std::map<int, Message /* msg id, Message*/> need_response_message_;
};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETSERVER_H_
