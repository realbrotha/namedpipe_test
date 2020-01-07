//
// Created by realbro on 12/8/19.
//

#ifndef NAMEDPIPEMANAGER_H_
#define NAMEDPIPEMANAGER_H_
#include "mutex.hpp"

#include <atomic>
#include <string>
#include <map>

struct PipeSingleInfo {
  int pipe_fd;
  std::string pipe_path;
};

struct PipePairInfo {
  struct PipeSingleInfo read;
  struct PipeSingleInfo write;
  pthread_t work_thread;
  pthread_t check_thread;
};

typedef void * (*ThreadPtr)(void *);

class NamedPipeManager {
 public:
  NamedPipeManager() { /* none */ };
  ~NamedPipeManager();

  bool Initialize(int type, bool mode, std::string path = "");

  static void EpollHandler(void* ptr);
  struct PipeSingleInfo read_pipe;

 private:

  int epollFd_;
  std::atomic<bool> stopped_;
  std::map<int, struct PipePairInfo> m_pipe_list;
};

struct CheckThreadArguments {
  NamedPipeManager *arg_this;
  std::string pipe_path;
  int type;
};

struct ThreadArguments {
  NamedPipeManager *arg_this;
  int code;
  bool mode;
};

#endif // NAMEDPIPEMANAGER_H_
