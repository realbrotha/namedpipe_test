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

  bool Initialize(int type, bool mode, std::string path = "");
  bool SendData(int type, std::string send_data);

  bool RemovePipeThreads(int type);

  bool RunCheckWritePipeThread(int type);
  bool CheckWritePipe(int type, std::string path);

 private:
  void set_pipe_info(int type, struct PipePairInfo st);
  bool get_pipe_info(int type, struct PipePairInfo &st);

  void set_pipe_thread_info(int type, pthread_t thread);

  void set_single_pipe_info(int type, int dest_type, struct PipeSingleInfo st);
  bool get_single_pipe_info(int type, int dest_type, struct PipeSingleInfo &st);

  static void *PipeThreadProc(void *arg);
  static void *WritePipeCheckThreadProc(void *arg);

  mutex m_mutex;
  std::map<int, struct PipePairInfo> m_pipe_list;
};

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
