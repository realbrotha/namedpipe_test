//
// Created by realbro on 1/2/20.
//

#include "StringTool.h"

#include <stdarg.h>
#include <string.h>

std::basic_string<char> StringTool::FormatString(const char *fmt, ...) {
  int length = 0;
  char *formatBuffer = NULL;
  std::basic_string<char> outTarget = "";
  va_list args;

  if (fmt == NULL) {
    return outTarget;
  }

  va_start(args, fmt);

  length = vsnprintf(NULL, 0, fmt, args);
  va_end(args);
  va_start(args, fmt);

  formatBuffer = new char[length + 1];
  memset(formatBuffer, 0, length + 1);
  vsnprintf(formatBuffer, length + 1, fmt, args);

  outTarget.assign(formatBuffer);
  if (formatBuffer != NULL)
    delete[] formatBuffer;

  va_end(args);

  return outTarget;
}