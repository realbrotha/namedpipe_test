//
// Created by realbro on 12/4/19.
//

#include "NamedPipeServer.h"

#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

NamedPipeServer &NamedPipeServer::GetInstance() {
  static NamedPipeServer instance_;
  return instance_;
}
NamedPipeServer::~NamedPipeServer() {
  std::cout << "destructor";
}
bool NamedPipeServer::Initialize(int count, std::string path) {
  bool result = false;
  if (count < 0) {
    std::cout << "code failed";
  }

  for (int i = 0; i < MAXIMUM_PRODUCT_COUNT; ++i) {
    if (count & (1 << i)) {
      MakePipeThreads(1 << i, (ThreadPtr *) PipeThreadProc, path);
      result = true;
    }
  }
  return result;
}
bool NamedPipeServer::Finalize() {
  //do something
}

// 쓰레드를 최전방에 노출시켜서 Generic 하게 사용하고자 했으나 좋은 방향 같지 않다.
void *NamedPipeServer::PipeThreadProc(void *arg) {
  ThreadArguments *args = reinterpret_cast<ThreadArguments *>(arg);
  NamedPipeManager *mgr = args->server_this;
  int product_type = static_cast<int>(args->code);
  delete args;

  std::cout << "=======ipc server thread======== product : " << product_type << "\n";
  char buffer_data[2048] = {0,};

  struct PipePairInfo st;
  while (mgr->get_pipe_info(product_type, st) && st.work_thread) {
    std::cout << "=======wait======== product : " << product_type << "\n";

    int read_size = read(st.send.pipe_fd, buffer_data, sizeof(buffer_data));
    if (read_size > 0) {
      std::string buffer(buffer_data);
      memset(buffer_data, 0x00, 2048);

      if (!st.recv.pipe_fd) {
        if (NamedPipeWrapper::connect(st.recv.pipe_path, st.recv.pipe_fd, false, true)) {
          std::cout << "connect recv pipe success\n";
        } else {
          std::cout << "connect recv pipe failed\n";
        }
      }
    } else if (read_size == 0 && st.recv.pipe_fd) {
      std::cout << "ipc recv pipe disconnect\n";
      NamedPipeWrapper::disconnect(st.recv.pipe_fd);

      st.recv.pipe_fd = 0;
      mgr->set_pipe_info(product_type, st);
    }
    sleep(1); //check async interval
  }
  if (st.send.pipe_fd) {

    std::cout << "disconnect " <<  NamedPipeWrapper::disconnect(st.send.pipe_fd);
    std::cout << "remove " <<     NamedPipeWrapper::remove(st.send.pipe_path);

  }
  if (st.recv.pipe_fd) {
    NamedPipeWrapper::disconnect(st.recv.pipe_fd);
    NamedPipeWrapper::remove(st.recv.pipe_path);
  }
  std::cout << "=======ipc server thread end======== product : " << product_type << "\n";
  pthread_exit(0);
  return NULL;
}
