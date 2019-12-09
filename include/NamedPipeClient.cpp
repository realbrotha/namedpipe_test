//
// Created by realbro on 12/8/19.
//

#include "NamedPipeClient.h"

#include <iostream>

NamedPipeClient& NamedPipeClient::GetInstance() {
  static NamedPipeClient instance_;
  return instance_;
}

bool NamedPipeClient::Initialize(int type, std::string path)
{
  std::string base_path = (!path.empty()) ? path : DEFAULT_PIPE_PATH;

  /*
  std::string send_pipe_path = base_path + PREFIX_SEND_PIPE_NAME + std::to_string(type);
  std::string recv_pipe_path = base_path + PREFIX_RECV_PIPE_NAME + std::to_string(type);

  if (NamedPipeWrapper::connect(send_pipe_path, pipeFd_, false, true) == 0 &&
      NamedPipeWrapper::connect(recv_pipe_path, recvPipeFd_, true, true) == 0 )
  {
    std::cout << "main &recv SUCCESS";
    return true;
  }
  else
  {
    std::cout << "client pipe connection failed.";
  }
   */
}