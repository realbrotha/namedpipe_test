//
// Created by realbro on 12/4/19.
//

#ifndef NAMED_PIPE_WRAPPER_H
#define NAMED_PIPE_WRAPPER_H

#include <string>

class named_pipe_wrapper
{
public:
    static bool create(const std::string &pipe_path, int mode = 0666);
    static bool connect(const std::string &pipe_path, int &fd, bool read_only, bool non_block);
    static bool disconnect(int &pipe);
    static bool remove(const std::string &pipe_path);
    static bool send(int send_pipe_fd, std::string send_string);
    static bool send_wait_response(int send_pipe_fd, std::string send_string, int recv_pipe_fd, int loop_count,
                       std::string &response_string);
};
#endif //NAMED_PIPE_WRAPPER_H
