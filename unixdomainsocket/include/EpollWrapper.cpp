//
// Created by realbro on 1/8/20.
//

#include "EpollWrapper.h"
#include "FileDescriptorTool.h"

#include <iostream>
#include <string>

bool EpollWrapper::EpollCreate(int epoll_size, bool blocking_mode, int &out_epoll_fd) {
  bool result = false;
  out_epoll_fd = epoll_create(epoll_size);
  if (out_epoll_fd < 0) {
    std::cout << "epoll wrapper create failed" << std::endl;
  } else {
    FileDescriptorTool::SetCloseOnExec(out_epoll_fd, blocking_mode);
    FileDescriptorTool::SetNonBlock(out_epoll_fd, blocking_mode);
    result = true;
  }
  return result;
}
bool EpollWrapper::EpollControll(int &epoll_fd, int target_fd, uint32_t epoll_event_type, int mode, void* data_struct) {
  struct epoll_event settingEvent = {0, {0}};

  if (data_struct){
    printf ("Epoll Controll!!!! struct");
    settingEvent.data.ptr = data_struct; //pointer ;
  } else {
    settingEvent.data.fd = target_fd;
  }
  settingEvent.events = epoll_event_type;

  return (-1 != epoll_ctl(epoll_fd, mode, target_fd, &settingEvent)) ? true : false;
}