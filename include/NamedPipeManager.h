//
// Created by realbro on 12/8/19.
//

#ifndef NAMEDPIPEMANAGER_H_
#define NAMEDPIPEMANAGER_H_
#include "mutex.hpp"

#include <string>
#include <map>

typedef void * (*ThreadPtr)(void *);

class NamedPipeManager {
 public:
  NamedPipeManager() { /* none */ };
  ~NamedPipeManager();

  void set_pipe_info(int type, struct PipePairInfo st);
  bool get_pipe_info(int type, struct PipePairInfo &st);

  void set_single_pipe_info(int type, int dest_type, struct PipeSingleInfo st);
  bool get_single_pipe_info(int type, int dest_type, struct PipeSingleInfo &st);

  bool RemovePipeThreads(int type);
  bool MakePipeThreads(int type, ThreadPtr *ptr, std::string path = "");
 private:
  mutex m_mutex;
  std::map<int, struct PipePairInfo> m_pipe_list;
};

struct PipeSingleInfo {
  int pipe_fd;
  std::string pipe_path;
};

struct PipePairInfo {
  struct PipeSingleInfo send;
  struct PipeSingleInfo recv;
  pthread_t work_thread;
};

struct ThreadArguments {
  NamedPipeManager *server_this;
  int code;
};

#endif //TESTPIPE_INCLUDE_NAMEDPIPEMANAGER_H_
