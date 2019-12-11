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

// type = product code (int)
// mode = client (false), server(true)
bool NamedPipeManager::Initialize(int type, bool mode, std::string path) {
  std::string base_path;
  base_path = (!path.empty()) ? path : DEFAULT_PIPE_PATH;

  umask(0000);
  std::string read_pipe_path;
  std::string write_pipe_path;

  if (mode) {
    read_pipe_path = base_path + PREFIX_SERVER_PIPE_NAME + std::to_string(type);
    write_pipe_path = base_path + PREFIX_CLIENT_PIPE_NAME + std::to_string(type);
  } else {
    read_pipe_path = base_path + PREFIX_CLIENT_PIPE_NAME + std::to_string(type);
    write_pipe_path = base_path + PREFIX_SERVER_PIPE_NAME + std::to_string(type);
  }

  std::cout << "pipe read string : " << read_pipe_path << "\n";
  std::cout << "pipe write string : " << write_pipe_path << "\n";

  // read
  struct PipeSingleInfo read = {0,};
  read.pipe_path = read_pipe_path;
  NamedPipeWrapper::create(read_pipe_path, 0666); // todo : 방어코드
  if (NamedPipeWrapper::connect(read_pipe_path, read.pipe_fd, true, false)) {
    std::cout << "read pipe connect success !!!!!!!!!\n";
  } else {
    std::cout << "read pipe connect failed. : " << errno << "\n";
    return false;
  }
  set_single_pipe_info(type, true, read);

  struct ThreadArguments *thread_arg = new ThreadArguments;
  thread_arg->arg_this = this;
  thread_arg->code = type;
  thread_arg->mode = mode;

  std::cout << "thread type : " << thread_arg->code << "\n";

  pthread_t thread_id;
  pthread_create(&thread_id, NULL, PipeThreadProc, thread_arg);
  set_pipe_thread_info(type, thread_id);

  // write
  // read
  struct PipeSingleInfo write = {0,};
  write.pipe_fd = 0;
  write.pipe_path = write_pipe_path;
  set_single_pipe_info(type, false, write);

  RunCheckWritePipeThread(type);
  printf("Initialize END~~~~~~~~~~~~~~~~~~~\n");
}

bool NamedPipeManager::SendData(int type, std::string send_data) {
  bool result = false;
  if (m_pipe_list.count(type)) {
    std::cout << "SendData~~!!\n";
    result = NamedPipeWrapper::send(m_pipe_list[type].write.pipe_fd, send_data);
  }
  return result;
}
bool NamedPipeManager::RemovePipeThreads(int type) {
  struct PipePairInfo st = {0,};
  if (m_pipe_list.count(type)) {
    pthread_t buffer = m_pipe_list[type].work_thread;
    m_pipe_list[type].work_thread = 0;
    pthread_join(buffer, NULL);
  }
}

bool NamedPipeManager::RunCheckWritePipeThread(int type) {
  printf("RunCheckWritePipeThread!!!!!!!!!!\n");

  PipeSingleInfo st = {0,};
  get_single_pipe_info(type, false, st);
  struct PipePairInfo st_buffer = {0,};
  struct CheckThreadArguments *check_thread_arg = new CheckThreadArguments;
  check_thread_arg->type = type;
  check_thread_arg->arg_this = this;
  check_thread_arg->pipe_path = st.pipe_path;
  printf("check target : %s\n", st.pipe_path.c_str());
  std::cout << "write pipe connect failed. check thread create\n";

  pthread_t check_thread;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  pthread_create(&check_thread, &attr, WritePipeCheckThreadProc, check_thread_arg);
  std::cout << "function fin\n";
}

bool NamedPipeManager::CheckWritePipe(int type, std::string path) {
  bool result = false;
  int fd = 0;
  printf("write pipe check !!!!\n");
  if (NamedPipeWrapper::connect(path, fd, false, true)) {
    printf("write pipe check !!!! Success\n");
    PipeSingleInfo st = {0,};
    st.pipe_fd = fd;
    st.pipe_path = path;

    set_single_pipe_info(type, false, st);
    result = true;
  }
  printf("write pipe check------END!!!!!! !!!!\n");
  return result;
}
void NamedPipeManager::set_pipe_thread_info(int type, pthread_t thread) {
  lock_guard<mutex> lock(m_mutex);
  if (!m_pipe_list.count(type)) {
    struct PipePairInfo base = {0,};
    m_pipe_list.insert(std::make_pair(type, base));
  }
  m_pipe_list[type].work_thread = thread;

}

void NamedPipeManager::set_pipe_info(int type, struct PipePairInfo st) {
  lock_guard<mutex> lock(m_mutex);
  if (!m_pipe_list.count(type)) {
    printf("set_pipe_info!!!!!!!!!!!!!");
    m_pipe_list.insert(std::make_pair(type, st));
  }
  m_pipe_list[type] = st;
}

bool NamedPipeManager::get_pipe_info(int type, struct PipePairInfo &st) {
  bool result = false;
  lock_guard<mutex> lock(m_mutex);
  if (m_pipe_list.count(type)) {
    st = m_pipe_list[type];
    result = true;
  }
  return result;
}

void NamedPipeManager::set_single_pipe_info(int type, int dest_type, struct PipeSingleInfo st) {
  printf("SetSingle Pipe!!!!!!!!!!!!!!!!\n");
  struct PipePairInfo base = {0,};
  lock_guard<mutex> lock(m_mutex);
  if (!m_pipe_list.count(type)) {
    m_pipe_list.insert(std::make_pair(type, base));
  }
  {
    if (dest_type) {
      m_pipe_list[type].read = st;
    } else {
      printf("write _fd set!!!!!!!!!***************\n");
      m_pipe_list[type].write = st;
    }
  }
  printf("SetSingle Pipe!!!!!!!!!!!!!!!! end\n");
}

bool NamedPipeManager::get_single_pipe_info(int type, int dest_type, struct PipeSingleInfo &st) {
  bool result = false;
  if (m_pipe_list.count(type)) {
    if (dest_type) {
      lock_guard<mutex> lock(m_mutex);
      if (m_pipe_list[type].read.pipe_fd &&
          !m_pipe_list[type].read.pipe_path.empty()) {
        st = m_pipe_list[type].read;
        result = true;
      }
    } else {
      lock_guard<mutex> lock(m_mutex);
      printf("write pipe setting\n");
      if (!m_pipe_list[type].write.pipe_path.empty()) {
        st = m_pipe_list[type].write;
        result = true;
      }
    }
  }
  return result;
}
void *NamedPipeManager::WritePipeCheckThreadProc(void *arg) {
  CheckThreadArguments *args = reinterpret_cast<CheckThreadArguments *>(arg);
  NamedPipeManager *mgr = args->arg_this;

  int product_type = args->type;
  std::string pipe_path = args->pipe_path;
  delete args;

  for (;;) {
    sleep(1);
    if (mgr->CheckWritePipe(product_type, pipe_path)) {
      printf("Write Pipe Connection Success!!! clear\n");
      break;
    }
  }
  printf("WritePipeCheckThreadProc exit!!!!!!!!\n");
  return NULL;
}
void *NamedPipeManager::PipeThreadProc(void *arg) {
  ThreadArguments *args = reinterpret_cast<ThreadArguments *>(arg);
  NamedPipeManager *mgr = args->arg_this;
  int product_type = args->code;
  bool mode = args->mode;
  delete args;

  char buffer_data[2048] = {0,};
  memset(buffer_data, 0x00, 2048);

  struct PipePairInfo st;
  while (mgr->get_pipe_info(product_type, st) && st.work_thread) {
    int read_size = read(st.read.pipe_fd, buffer_data, sizeof(buffer_data));
    //printf("read_size : %d\n", read_size);
    if (read_size > 0) {
      std::string buffer(buffer_data);
      memset(buffer_data, 0x00, 2048);
      printf( "recv buffer data : %s\n", buffer.c_str());
    } else if (read_size == 0 && st.write.pipe_fd) {
      printf("read size : (%d), write.pipe_fd :(%d)\n", read_size, st.write.pipe_fd);
      std::cout << "ipc write pipe disconnect\n";
      NamedPipeWrapper::disconnect(st.write.pipe_fd);
      st.write.pipe_fd = 0;
      mgr->set_pipe_info(product_type, st);
      mgr->RunCheckWritePipeThread(product_type);
    }
    //printf("hehre ??? -2\n");
  }
  std::cout << "=======ipc server thread end======== product : " << product_type << "\n";

  if (st.read.pipe_fd) {
    NamedPipeWrapper::disconnect(st.read.pipe_fd);
    NamedPipeWrapper::remove(st.read.pipe_path);
  }
  if (st.write.pipe_fd) {
    NamedPipeWrapper::disconnect(st.write.pipe_fd);
    NamedPipeWrapper::remove(st.write.pipe_path);
  }
  return NULL;
}
