//
// Created by realbro on 12/4/19.
//

#ifndef NAMED_PIPE_MANAGER_H
#define NAMED_PIPE_MANAGER_H

#include "NamedPipeWrapper.h"
#include "NamedPipeManager.h"

#include <stdint.h>
#include <pthread.h>
#include <map>

#define MAXIMUM_PRODUCT_COUNT 3

class NamedPipeServer : public NamedPipeWrapper ,
                        public NamedPipeManager
{
 public :
  static NamedPipeServer& GetInstance();

  bool Initialize(int count, std::string path = "");
  bool Finalize();

  ~NamedPipeServer();
 private :
  NamedPipeServer() { /* none */ };

  static void *PipeThreadProc(void *arg); // Impl Thread Proc
};


#endif //NAMED_PIPE_MANAGER_H
