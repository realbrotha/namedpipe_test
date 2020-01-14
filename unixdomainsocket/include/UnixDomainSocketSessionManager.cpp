//
// Created by realbro on 1/7/20.
//

#include "UnixDomainSocketSessionManager.h"

#include <iostream>
#include <sys/un.h>

UnixDomainSocketSessionManager &UnixDomainSocketSessionManager::GetInstance() {
  static UnixDomainSocketSessionManager instance_;
  return instance_;
}

UnixDomainSocketSessionManager::~UnixDomainSocketSessionManager() {
  // todo : do somthing
  lock_guard<mutex> lock(mutex_);
  RemoveAll();
}

bool UnixDomainSocketSessionManager::Add(int& product_code, int &socket_fd) {
  bool result = false;
  if (socket_list_.count(product_code)) {
    std::cout << "already exist";
  } else {
    lock_guard<mutex> lock(mutex_);
    socket_list_[product_code] = socket_fd;
    result = true;
  }
  return result;
}

MessageManager &UnixDomainSocketSessionManager::GetMessageManager() {
  return message_manager_;
}

std::map<int, int> UnixDomainSocketSessionManager::GetAllSession() {
  std::map<int, int> buffer;
  if (!socket_list_.empty()) {
    buffer = socket_list_;
  }
  return buffer;
}

bool UnixDomainSocketSessionManager::Remove(int &socket_fd) {
  bool result = false;

  if (socket_list_.count(socket_fd)) {
    lock_guard<mutex> lock(mutex_);
    socket_list_.erase(socket_fd);
    result = true;
  }
  return result;
}

bool UnixDomainSocketSessionManager::RemoveAll() {
  socket_list_.clear();
}

