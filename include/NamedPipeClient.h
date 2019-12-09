//
// Created by realbro on 12/8/19.
//

#ifndef NAMEDPIPECLIENT_H_
#define NAMEDPIPECLIENT_H_

#include "NamedPipeWrapper.h"

class NamedPipeClient : public NamedPipeWrapper{
 public:
  static NamedPipeClient& GetInstance();

  bool Initialize(int type, std::string path = "");
  void Finalize();

 private:
  NamedPipeClient() { /* none */ };
};

#endif //NAMEDPIPECLIENT_H_
