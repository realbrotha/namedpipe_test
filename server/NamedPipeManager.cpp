//
// Created by realbro on 12/4/19.
//

#include "NamedPipeManager.h"

#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

NamedPipeManager &NamedPipeManager::get_instance() {
  static NamedPipeManager instance_;
  return instance_;
}

bool NamedPipeManager::Initialize(int count, std::string path) {
  bool result = false;
  if (count < 0) {
    std::cout << "code failed";
  }

  for (int i = 0; i < MAXIMUM_PRODUCT_COUNT; ++i) {
    if (count & (1 << i)) {
      MakePipeThreads(1 << i);
      result = true;
    }
  }
  return result;
}
bool NamedPipeManager::Finalize() {
  //do something
}

bool NamedPipeManager::MakePipeThreads(int code, std::string path) {
  std::string base_path;
  base_path = (!path.empty()) ? path : "/tmp";

  umask(0000);
  const std::string kprefix_send_name = ".send";
  std::string send_pipe_path = base_path + "/" + kprefix_send_name + std::to_string(code);

  int send_fd = 0;

  struct PipeSingleInfo send;
  NamedPipeWrapper::remove(send_pipe_path);

  if (NamedPipeWrapper::create(send_pipe_path, 0666) &&
      NamedPipeWrapper::connect(send_pipe_path, send_fd, true, true)) {
    send.pipe_fd = send_fd;
    send.pipe_path = send_pipe_path;

    set_single_pipe_info(code, 1, send);

    std::cout << "sender success\n";
  } else {
    std::cout << "sender failed.\n";
    return false;
  }
  const std::string kprefix_recv_name = ".recv";
  std::string recv_pipe_path = base_path + "/" + kprefix_recv_name + std::to_string(code);

  struct PipeSingleInfo recv;
  NamedPipeWrapper::remove(recv_pipe_path);
  if (NamedPipeWrapper::create(recv_pipe_path, 0666)) {
    std::cout << "create recv pipe success\n";
    recv.pipe_fd = 0;
    recv.pipe_path = recv_pipe_path;
    set_single_pipe_info(code, 0, recv);
  } else {
    std::cout << "recv failed.\n";
    return false;
  }

  pthread_t thread_buffer;

  struct ThreadArguments *thread_arg = new ThreadArguments;

  thread_arg->manager_this = this;
  thread_arg->code = code;

  std::cout << "thread code : " << thread_arg->code << code;
  pthread_create(&thread_buffer, NULL, PipeThreadProc, thread_arg);

  struct PipePairInfo st_buffer;
  st_buffer.send = send;
  st_buffer.recv = recv;
  st_buffer.work_thread = thread_buffer;

  std::cout << "Initialize finish\n";
}

void NamedPipeManager::set_pipe_info(int code, struct PipePairInfo &st) {
  m_pipe_list.insert(std::make_pair(code, st));
}

bool NamedPipeManager::get_pipe_info(int code, struct PipePairInfo &st) {
  bool result = false;

  if (m_pipe_list.count(code)) {
    st = m_pipe_list[code];
    result = true;
  }
  return result;
}

void NamedPipeManager::set_single_pipe_info(int code, int type, struct PipeSingleInfo &st) {
  struct PipePairInfo base;
  if (!m_pipe_list.count(code)) {
    m_pipe_list.insert(std::make_pair(code, base));
  }
  if (type) // send
    m_pipe_list[code].send = st;
  else
    m_pipe_list[code].recv = st;
}

bool NamedPipeManager::get_single_pipe_info(int code, int type, struct PipeSingleInfo &st) {
  bool result = false;
  if (m_pipe_list.count(code)) {
    if (type) // send
    {
      if (m_pipe_list[code].send.pipe_fd &&
          !m_pipe_list[code].send.pipe_path.empty()) {
        st = m_pipe_list[code].send;
        result = true;
      }
    } else // recv
    {
      if (m_pipe_list[code].recv.pipe_fd &&
          !m_pipe_list[code].recv.pipe_path.empty()) {
        st = m_pipe_list[code].recv;
        result = true;
      }
    }
  }
  return result;
}

void *NamedPipeManager::PipeThreadProc(void *arg) {
  ThreadArguments *args = reinterpret_cast<ThreadArguments *>(arg);
  NamedPipeManager *mgr = args->manager_this;
  int product_type = static_cast<int>(args->code);
  delete args;

  std::cout << "=======ipc server thread======== product : " << product_type << "\n";
  char buffer_data[2048] = {0,};

  struct PipePairInfo st;
  while (mgr->get_pipe_info(product_type, st)) {
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
    NamedPipeWrapper::disconnect(st.send.pipe_fd);
    NamedPipeWrapper::remove(st.send.pipe_path);
  }
  if (st.recv.pipe_fd) {
    NamedPipeWrapper::disconnect(st.recv.pipe_fd);
    NamedPipeWrapper::remove(st.recv.pipe_path);
  }
  std::cout << "=======ipc server thread end======== product : " << product_type << "\n";
  pthread_exit(0);
  return NULL;
}
