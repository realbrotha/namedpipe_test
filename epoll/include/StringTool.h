//
// Created by realbro on 1/2/20.
//

#ifndef EPOLL_INCLUDE_STRINGTOOL_H_
#define EPOLL_INCLUDE_STRINGTOOL_H_

#include <string>

class StringTool {
 public:
  std::basic_string<char> FormatString(const char *fmt, ...) __attribute__((format (printf, 2, 3)));
};

#endif //EPOLL_INCLUDE_STRINGTOOL_H_
