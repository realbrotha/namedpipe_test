//
// Created by realbro on 12/4/19.
//

#ifndef NAMED_PIPE_MANAGER_H
#define NAMED_PIPE_MANAGER_H

#include "named_pipe_wrapper.h"

#include <stdint.h>
#include <pthread.h>
#include <map>

#define MAXIMUM_PRODUCT_COUNT 3

class named_pipe_manager : public named_pipe_wrapper
{
public :
    static named_pipe_manager& get_instance();

    bool initialize(int count, std::string path = "");
    bool finalize();

    void set_pipe_info(int code, struct pipe_info &st);
    bool get_pipe_info(int code, struct pipe_info &st);

    void set_single_pipe_info(int code, int type, struct pipe_single &st);
    bool get_single_pipe_info(int code, int type, struct pipe_single &st);
private :
    named_pipe_manager() { /* none */ };

    bool make_pipe_thread(int code, std::string path = "");

    static void* server_proc(void* arg);
    std::map<int, struct pipe_info> m_pipe_list;
};

struct pipe_single
{
    int pipe_fd;
    std::string pipe_path;
};
struct pipe_info
{
    struct pipe_single send;
    struct pipe_single recv;
    pthread_t work_thread;
};

struct work_thread_args
{
    named_pipe_manager *manager_this;
    int code;
};


#endif //NAMED_PIPE_MANAGER_H
