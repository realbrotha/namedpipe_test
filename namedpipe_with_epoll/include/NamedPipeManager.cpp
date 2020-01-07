//
// Created by realbro on 12/8/19.
//

#include "NamedPipeManager.h"
#include "NamedPipeWrapper.h"
#include "FileDescriptorTool.h"

#include <thread>
#include <iostream>

#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/epoll.h>

NamedPipeManager::~NamedPipeManager() {

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
    read_pipe = read;
    printf("read pipe connect success !!!!!!!!! fd : (%d)\n", read_pipe.pipe_fd);
  } else {
    std::cout << "read pipe connect failed. : " << errno << "\n";
    return false;
  }

  int epollFd = epoll_create(1);
  if (epollFd < 0) {
    std::cout << "EpollHandler::Initialize failed";
    return false;
  }
  FileDescriptorTool::SetCloseOnExec(epollFd, true);
  FileDescriptorTool::SetNonBlock(epollFd, true);

  epollFd_ = epollFd;

  epoll_event settingEvent = {0, {0}};
  settingEvent.data.fd = read.pipe_fd;
  settingEvent.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET | EPOLLRDHUP;

  if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, read.pipe_fd, &settingEvent)) {
    std::cout << "NamedPipeManager::HandleMain: epoll_ctl" << std::endl;
    return false;
  }

  std::thread read_thread(&NamedPipeManager::EpollHandler, this);
  read_thread.join();

  printf("Initialize Finish\n");
  return true;
}

void NamedPipeManager::EpollHandler(void *ptr) {
  NamedPipeManager *mgr = reinterpret_cast<NamedPipeManager *>(ptr);
  std::cout << "EpollHandler Thread!!!!" << std::endl;
  mgr->stopped_ = false;
  const int kMAX_EVENT_COUNT = 3;
  while (!mgr->stopped_) {
    epoll_event gettingEvent[kMAX_EVENT_COUNT] = {0, {0}};
    int rc = epoll_wait(mgr->epollFd_, gettingEvent, kMAX_EVENT_COUNT, 10 /* ms */);
    int errorCount = 0;

  //  std::cout << "EpollHandler::HandleMain: epoll_: " << rc << std::endl;
    if (rc < 0) // error
    {
      if (errno == EINTR) {
        continue;
      }

      std::cout << "EpollHandler::HandleMain: epoll_wait" << std::endl;
      ++errorCount;
      if (errorCount < 3) {
        continue;
      }
      return;
    }
    if (rc > 0) // event ok...
    {
      for ( int i = 0 ; i < rc ; ++i) {
        printf("loop : %d\n",i);
        printf("epoll event : %d\n", gettingEvent[i].events);
        printf("epoll fd : %d\n", gettingEvent[i].data.fd);
        if ( mgr->read_pipe.pipe_fd == gettingEvent[i].data.fd)
        {
          printf ("lulu!!!!!!!!!!!!!!!!!\n");
        }

        // event
        char buffff[1024] = {0,};
        int read_size = read(mgr->read_pipe.pipe_fd, buffff, sizeof(buffff));
        printf("read_size : %d\n", read_size);
        if (read_size > 0) {
          std::string buffer(buffff);
          memset(buffff, 0x00, 2048);
          printf("recv buffer data : %s\n", buffer.c_str());
        }
      }
    }
  }
  std::cout << "EpollHandler Thread!!!! END OF Thread" << std::endl;
}


