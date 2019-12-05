//
// Created by realbro on 12/4/19.
//

#include "named_pipe_wrapper.h"

#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int named_pipe_wrapper::create(const std::basic_string<char>& pipe_path, int mode)
{
    if (pipe_path.empty())
    {
        std::cout <<"pipe path wrong";
        return -1;
    }
    return mkfifo(pipe_path.c_str(), mode);
}

int named_pipe_wrapper::connect(const std::basic_string<char>& pipe_path, bool read_only, bool non_block)
{
    if (pipe_path.empty())
    {
        std::cout <<"pipe path wrong";
        return -1;
    }
    return open(pipe_path.c_str(), (read_only ? O_RDONLY : O_WRONLY) | (non_block ? O_NONBLOCK : 0));
}

int named_pipe_wrapper::disconnect(int& pipe)
{
    if (0 >= pipe )
    {
        std::cout << "wrong fd";
    }
    return close(pipe);
}

int named_pipe_wrapper::remove(const std::basic_string<char>& pipePath)
{
    return unlink(pipePath.c_str());
}

bool named_pipe_wrapper::send(int send_pipe_fd,  std::basic_string<char> send_string)
{
    bool result = false;
    if (0 >= send_pipe_fd )
    {
        return result;
    }
    if(write(send_pipe_fd, send_string.c_str(), send_string.length()) != -1)
    {
        result = true;
    }

    return result;
}

bool named_pipe_wrapper::send_wait_response(int send_pipe_fd,  std::basic_string<char> send_string, int recv_pipe_fd, int loop_count, std::basic_string<char>& response_string)
{
    bool result = false;
    if (0 >= send_pipe_fd && 0 >= recv_pipe_fd)
    {
        std::cout << "wrong descript";
        return result;
    }

    if(write(send_pipe_fd, send_string.c_str(), send_string.length()) != -1)
    {
        while (loop_count--)
        {
            int readSize = 0;
            char buffer[2048] = {0,};

            if ((readSize = read(recv_pipe_fd, buffer, sizeof(buffer))) > 0)
            {
                response_string.assign(buffer,readSize);
                result = true;
                break;
            }
            else
            {
                usleep(1);
                continue;
            }
        }
    }
    return result;
}