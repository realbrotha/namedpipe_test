#ifndef MUTEX_H
#define MUTEX_H

#include <errno.h>
#include <pthread.h>
#include <string.h>

template<typename T>
class lock_guard {
 public :
  lock_guard(T &mutex) : m_(mutex) {
    m_.lock();
  }
  ~lock_guard() {
    m_.unlock();
  }
 private:
  T &m_;
};
#include <stdio.h>
class mutex {
 public:
  mutex() {
    if (pthread_mutex_init(&mu_, NULL) != 0) {
      printf("mutex error");
      // TODO : errro handler
    }
  }
  void lock() {
    pthread_mutex_lock(&mu_);
  }
  void unlock() {
    pthread_mutex_unlock(&mu_);
  }
  ~mutex() {
    if (0 != pthread_mutex_destroy(&mu_)) {
      if (errno == EBUSY) {
        unlock();
        pthread_mutex_destroy(&mu_);
      }
    }
  }
 private:
  pthread_mutex_t mu_;
};
#endif // MUTEX_H