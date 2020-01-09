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

bool UnixDomainSocketSessionManager::Add(int &socket_fd, struct sockaddr_un &addr) {
  bool result = false;

  if (alive_client_session.count(socket_fd)) {
    std::cout << "already exist";
  } else {
    lock_guard<mutex> lock(mutex_);
    alive_client_session[socket_fd] = addr;
    result = true;
  }
  return result;
}

std::map<int, struct sockaddr_un> UnixDomainSocketSessionManager::GetAll() {
  std::map<int, struct sockaddr_un> buffer;

  if (alive_client_session.size()) {
    buffer = alive_client_session;
  }
  return buffer;
}

bool UnixDomainSocketSessionManager::Remove(int &socket_fd) {
  bool result = false;

  if (alive_client_session.count(socket_fd)) {
    lock_guard<mutex> lock(mutex_);
    alive_client_session.erase(socket_fd);
    result = true;
  }
  return result;
}
bool UnixDomainSocketSessionManager::RemoveAll() {
  alive_client_session.clear();
}

