//
// Created by realbro on 1/2/20.
//

#ifndef EPOLL_INCLUDE_FILEDESCRIPTORTOOL_H_
#define EPOLL_INCLUDE_FILEDESCRIPTORTOOL_H_

#include <string>

namespace {
const int kMaxBufferSize = 1024;
} // end of namespace

class FileDescriptorTool {
 public:
  static int SetCloseOnExec(int fd, bool closeOnExec);
  static int SetNonBlock(int fd, bool nonBlock);
  static std::basic_string<char> ReadLink(int fd);
};

#endif //EPOLL_INCLUDE_FILEDESCRIPTORTOOL_H_
