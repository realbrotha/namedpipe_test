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

bool UnixDomainSocketSessionManager::Add(int &product_code, int &socket_fd) {
  bool result = false;
  if (socket_list_.count(product_code)) {
    std::cout << "already exist";
  } else {
    lock_guard<mutex> lock(mutex_);
    reverse_socket_list_[socket_fd] = product_code;
    socket_list_[product_code] = socket_fd;
    result = true;
  }
  return result;
}
bool UnixDomainSocketSessionManager::GetSocketFdAll(std::map<int, int>& socket_list) {

  if (!socket_list_.empty()) {
    socket_list = socket_list_;
    return true;
  }
  return false;
}

int32_t UnixDomainSocketSessionManager::GetSocketFdSize()
{
  return socket_list_.size();
}

bool UnixDomainSocketSessionManager::Remove(int &product_code) {
  bool result = false;

  if (socket_list_.count(product_code)) {
    lock_guard<mutex> lock(mutex_);
    reverse_socket_list_.erase(socket_list_[product_code]);
    socket_list_.erase(product_code);
    result = true;
  }
  return result;
}

bool UnixDomainSocketSessionManager::RemoveAll() {
  socket_list_.clear();
  reverse_socket_list_.clear();
}

bool UnixDomainSocketSessionManager::GetProductCode(int &product_code, int &socket_fd) {
  bool result =false;
  if (reverse_socket_list_.count(socket_fd)) {
    lock_guard<mutex> lock(mutex_);
    product_code = reverse_socket_list_[socket_fd];
    result = true;
  }
  return result;
}
bool UnixDomainSocketSessionManager::GetSocketFd(int &product_code, int &socket_fd) {
  bool result = false;
  if (socket_list_.count(product_code)) {
    lock_guard<mutex> lock(mutex_);
    socket_fd = socket_list_[product_code];
    result = true;
  }
  return result;
};