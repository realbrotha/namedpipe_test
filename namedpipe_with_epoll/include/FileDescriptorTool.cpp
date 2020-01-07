//
// Created by realbro on 1/2/20.
//

#include "FileDescriptorTool.h"
#include "StringTool.h"

#include <array>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int FileDescriptorTool::SetCloseOnExec(int fd, bool closeOnExec) {
  int val = 0;

  val = fcntl(fd, F_GETFD, 0);
  if (val < 0) {
    return val;
  }
  if (closeOnExec) {
    val |= FD_CLOEXEC;
  } else {
    val &= ~FD_CLOEXEC;
  }

  return fcntl(fd, F_SETFD, val);
}

int FileDescriptorTool::SetNonBlock(int fd, bool nonBlock) {
  int val = 0;

  val = fcntl(fd, F_GETFL, 0);
  if (val < 0) {
    return val;
  }
  if (nonBlock) {
    val |= O_NONBLOCK;
  } else {
    val &= ~O_NONBLOCK;
  }
  return fcntl(fd, F_SETFL, val);
}

std::basic_string<char> FileDescriptorTool::ReadLink(int fd) {
  std::array<char, kMaxBufferSize + 1> buffer = {};
  size_t readSize = buffer.size();

  ssize_t result = readlink(StringTool().FormatString("/proc/self/fd/%d", fd).c_str(), buffer.data(), readSize);
  if (result < 0) {
    return "";
  }

  result = std::min(result, static_cast<ssize_t>(buffer.size() - 1));
  buffer[result] = '\0';

  return std::basic_string<char>(buffer.data(), static_cast<size_t>(result));
}