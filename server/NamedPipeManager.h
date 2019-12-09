//
// Created by realbro on 12/4/19.
//

#ifndef NAMED_PIPE_MANAGER_H
#define NAMED_PIPE_MANAGER_H

#include "NamedPipeWrapper.h"

#include <stdint.h>
#include <pthread.h>
#include <map>

#define MAXIMUM_PRODUCT_COUNT 3

class NamedPipeManager : public NamedPipeWrapper {
 public :
  static NamedPipeManager &get_instance();

  bool Initialize(int count, std::string path = "");
  bool Finalize();

  void set_pipe_info(int code, struct PipePairInfo &st);
  bool get_pipe_info(int code, struct PipePairInfo &st);

  void set_single_pipe_info(int code, int type, struct PipeSingleInfo &st);
  bool get_single_pipe_info(int code, int type, struct PipeSingleInfo &st);
 private :
  NamedPipeManager() { /* none */ };

  bool MakePipeThreads(int code, std::string path = "");

  static void *PipeThreadProc(void *arg);
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
  NamedPipeManager *manager_this;
  int code;
};

#endif //NAMED_PIPE_MANAGER_H
