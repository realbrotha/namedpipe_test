//
// Created by realbro on 1/2/20.
//

#include "EpollHandler.h"
#include "FileDescriptorTool.h"

#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/epoll.h>

namespace {
const int kRawBufferSize = 8192;
} // end of namespace

EpollHandler::EpollHandler()
    : notifyFd_(-1)
    , epollFd_(-1)
    , stopped_(true)
{
}

EpollHandler::~EpollHandler()
{
  Uninitialize();
}

bool EpollHandler::Initialize(int fd)
{
  if (fd < 0)
  {
    std::cout << "EpollHandler::Initialize failed";
    return false;
  }

  if (notifyFd_ != -1)
  {
    if (fd == notifyFd_)
    {
      return true;
    }
    std::cout << "EpollHandler::Initialize failed: origin fd: " << notifyFd_ << " , input fd: " << fd << std::endl;
    return false;
  }

  int epollFd = epoll_create(1);
  if (epollFd < 0)
  {
    std::cout << "EpollHandler::Initialize failed";
    return false;
  }

  FileDescriptorTool::SetCloseOnExec(epollFd, true);
  FileDescriptorTool::SetNonBlock(epollFd, true);

  notifyFd_ = fd;
  epollFd_ = epollFd;
  stopped_ = false;

  return true;
}

void EpollHandler::Uninitialize()
{
  Stop();

  if (epollFd_ != -1)
  {
    close(epollFd_);
    epollFd_ = -1;
  }

  if (notifyFd_ != -1)
  {
    close(notifyFd_);
    notifyFd_ = -1;
  }
}

void EpollHandler::Stop()
{
  stopped_ = true;
}

void EpollHandler::HandlerMain()
{
  epoll_event settingEvent = { 0, { 0 } };
  settingEvent.events = EPOLLIN;

  if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, notifyFd_, &settingEvent))
  {
    std::cout << "EpollHandler::HandleMain: epoll_ctl" << std::endl;
    return;
  }

  while (!stopped_)
  {
    epoll_event gettingEvent = {0, {0}};
    int rc = epoll_wait(epollFd_, &gettingEvent, 1, 10 /* ms */);
    int errorCount = 0;
    if (rc < 0)
    {
      if (errno == EINTR)
      {
        continue;
      }

      std::cout << "EpollHandler::HandleMain: epoll_wait" << std::endl;
      ++errorCount;
      if (errorCount < 3)
      {
        continue;
      }
      return;
    }

    // event 아마 이쯤 올거고 ...
    if (rc > 0)
    {
      // event
    }

    while (true) {
    }
  }
}