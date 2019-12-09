//
// Created by realbro on 12/8/19.
//

#include "NamedPipeManager.h"
#include "NamedPipeWrapper.h"

#include <unistd.h>
#include <sys/stat.h>
#include <iostream>

#include <signal.h>

NamedPipeManager::~NamedPipeManager() {
  for (auto it = m_pipe_list.begin(); it != m_pipe_list.end(); it++) {
    RemovePipeThreads(it->first);
  }
}
void NamedPipeManager::set_pipe_info(int type, struct PipePairInfo st) {
  lock_guard<mutex> lock(m_mutex);
  if (m_pipe_list.count(type)) {
    m_pipe_list[type] = st;
  } else {
    m_pipe_list.insert(std::make_pair(type, st));
  }
}

bool NamedPipeManager::get_pipe_info(int type, struct PipePairInfo &st) {
  bool result = false;
  if (m_pipe_list.count(type)) {
    lock_guard<mutex> lock(m_mutex);
    st = m_pipe_list[type];
    result = true;
  }
  return result;
}

void NamedPipeManager::set_single_pipe_info(int type, int dest_type, struct PipeSingleInfo st) {
  struct PipePairInfo base;
  if (!m_pipe_list.count(type)) {
    lock_guard<mutex> lock(m_mutex);
    m_pipe_list.insert(std::make_pair(type, base));
  }
  {
    lock_guard<mutex> lock(m_mutex);
    if (dest_type) // send
      m_pipe_list[type].send = st;
    else // recv
      m_pipe_list[type].recv = st;
  }
}

bool NamedPipeManager::get_single_pipe_info(int type, int dest_type, struct PipeSingleInfo &st) {
  bool result = false;
  if (m_pipe_list.count(type)) {
    if (dest_type) // send
    {
      lock_guard<mutex> lock(m_mutex);
      if (m_pipe_list[type].send.pipe_fd &&
          !m_pipe_list[type].send.pipe_path.empty()) {
        st = m_pipe_list[type].send;
        result = true;
      }
    } else // recv
    {
      lock_guard<mutex> lock(m_mutex);
      if (m_pipe_list[type].recv.pipe_fd &&
          !m_pipe_list[type].recv.pipe_path.empty()) {
        st = m_pipe_list[type].recv;
        result = true;
      }
    }
  }
  return result;
}
bool NamedPipeManager::RemovePipeThreads(int type) {
  struct PipePairInfo st;
  if (m_pipe_list.count(type)) {
    pthread_t buffer = m_pipe_list[type].work_thread;
    m_pipe_list[type].work_thread = 0;
    pthread_join(buffer, NULL);
  }
}
bool NamedPipeManager::MakePipeThreads(int type, ThreadPtr *ptr, std::string path) {
  std::string base_path;
  base_path = (!path.empty()) ? path : DEFAULT_PIPE_PATH;

  umask(0000);
  std::string send_pipe_path = base_path + PREFIX_SEND_PIPE_NAME + std::to_string(type);

  int send_fd = 0;

  struct PipeSingleInfo send;
  NamedPipeWrapper::remove(send_pipe_path);

  if (NamedPipeWrapper::create(send_pipe_path, 0666) &&
      NamedPipeWrapper::connect(send_pipe_path, send_fd, true, true)) {
    send.pipe_fd = send_fd;
    send.pipe_path = send_pipe_path;

    set_single_pipe_info(type, 1, send);

    std::cout << "sender success\n";
  } else {
    std::cout << "sender failed.\n";
    return false;
  }
  std::string recv_pipe_path = base_path + PREFIX_RECV_PIPE_NAME + std::to_string(type);

  struct PipeSingleInfo recv;
  NamedPipeWrapper::remove(recv_pipe_path);
  if (NamedPipeWrapper::create(recv_pipe_path, 0666)) {
    std::cout << "create recv pipe success\n";
    recv.pipe_fd = 0;
    recv.pipe_path = recv_pipe_path;
    set_single_pipe_info(type, 0, recv);
  } else {
    std::cout << "recv failed.\n";
    return false;
  }
  struct ThreadArguments *thread_arg = new ThreadArguments;
  thread_arg->server_this = this;
  thread_arg->code = type;

  std::cout << "thread type : " << thread_arg->code << "\n";
  struct PipePairInfo st_buffer;
  st_buffer.send = send;
  st_buffer.recv = recv;

  pthread_create(&st_buffer.work_thread, NULL, reinterpret_cast<ThreadPtr>(ptr), thread_arg);
  set_pipe_info(type, st_buffer);
  std::cout << "Initialize finish\n";
}
