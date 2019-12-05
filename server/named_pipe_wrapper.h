//
// Created by realbro on 12/4/19.
//

#ifndef NAMED_PIPE_WRAPPER_H
#define NAMED_PIPE_WRAPPER_H

#include <string>

class named_pipe_wrapper
{
public:
    static int create(const std::basic_string<char>& pipe_path, int mode = 0666);
    static int connect(const std::basic_string<char>& pipe_path, bool read_only, bool non_block);
    static int disconnect(int& pipe);
    static int remove(const std::basic_string<char>& pipe_path);

    static bool send(int send_pipe_fd, std::basic_string<char> send_string);
    static bool send_wait_response(int send_pipe_fd,  std::basic_string<char> send_string, int recv_pipe_fd, int loop_count, std::basic_string<char>& response_string);
};

#endif //NAMED_PIPE_WRAPPER_H
