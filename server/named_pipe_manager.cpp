//
// Created by realbro on 12/4/19.
//

#include "named_pipe_manager.h"

#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

named_pipe_manager& named_pipe_manager::get_instance()
{
    static named_pipe_manager instance_;
    return instance_;
}

bool named_pipe_manager::initialize(int code, std::string path)
{
    bool result = false;
    if ( code < 0)
    {
        std::cout << "code failed";
    }

    for (int i = 0 ; i < MAXIMUM_PRODUCT_COUNT; ++i)
    {
        if ( code & (1 << i))
        {
            make_pipe_thread(1 << i);
            result = true;
        }
    }
    return result;
}
bool named_pipe_manager::finalize()
{
    //do something
}

bool named_pipe_manager::make_pipe_thread(int code, std::string path)
{
    std::string base_path;
    base_path = (!path.empty()) ? path : "/tmp";

    umask(0000);
    const std::string kprefix_send_name = ".send";
    std::string send_pipe_path = base_path + "/" + kprefix_send_name + std::to_string(code);

    int send_fd = 0;

    struct pipe_single send;
    named_pipe_wrapper::remove(send_pipe_path);
    if (named_pipe_wrapper::create(send_pipe_path, 0666) &&
        named_pipe_wrapper::connect(send_pipe_path, send_fd, true, true))
    {
        send.pipe_fd = send_fd;
        send.pipe_path = send_pipe_path;

        set_single_pipe_info(code, 1, send);

        std::cout << "sender success\n";
    }
    else
    {
        std::cout << "sender failed.\n";
        return false;
    }
    const std::string kprefix_recv_name = ".recv";
    std::string recv_pipe_path = base_path + "/" + kprefix_recv_name + std::to_string(code);

    struct pipe_single recv;
    named_pipe_wrapper::remove(recv_pipe_path);
    if (named_pipe_wrapper::create(recv_pipe_path, 0666))
    {
        std::cout << "create recv pipe success\n";
        recv.pipe_fd = 0;
        recv.pipe_path = recv_pipe_path;
        set_single_pipe_info(code, 0, recv);
    }
    else
    {
        std::cout << "recv failed.\n";
        return false;
    }

    pthread_t thread_buffer;

    struct work_thread_args *thread_arg = new work_thread_args;

    thread_arg->manager_this = this;
    thread_arg->code = code;

    std::cout << "thread code : " << thread_arg->code << code;
    pthread_create(&thread_buffer, NULL, server_proc, thread_arg);

    struct pipe_info st_buffer;
    st_buffer.send = send;
    st_buffer.recv = recv;
    st_buffer.work_thread = thread_buffer;

    std::cout  << "initialize finish\n";
}

void named_pipe_manager::set_pipe_info(int code, struct pipe_info &st)
{
    m_pipe_list.insert(std::make_pair(code, st));
}

bool named_pipe_manager::get_pipe_info(int code, struct pipe_info &st)
{
    bool result = false;

    if(m_pipe_list.count(code))
    {
        st = m_pipe_list[code];
        result = true;
    }
    return result;
}

void named_pipe_manager::set_single_pipe_info(int code, int type, struct pipe_single &st)
{
    struct pipe_info base;
    if(!m_pipe_list.count(code))
    {
        m_pipe_list.insert(std::make_pair(code, base));
    }
    if (type) // send
        m_pipe_list[code].send = st;
    else
        m_pipe_list[code].recv = st;
}

bool named_pipe_manager::get_single_pipe_info(int code, int type, struct pipe_single &st)
{
    bool result = false;
    if(m_pipe_list.count(code))
    {
        if (type) // send
        {
            if (m_pipe_list[code].send.pipe_fd &&
                !m_pipe_list[code].send.pipe_path.empty())
            {
                st = m_pipe_list[code].send;
                result = true;
            }
        }
        else // recv
        {
            if (m_pipe_list[code].recv.pipe_fd &&
                !m_pipe_list[code].recv.pipe_path.empty())
            {
                st = m_pipe_list[code].recv;
                result = true;
            }
        }
    }
    return result;
}

void* named_pipe_manager::server_proc(void* arg)
{
    work_thread_args* args = reinterpret_cast<work_thread_args*>(arg);
    named_pipe_manager* mgr = args->manager_this;
    int product_type = static_cast<int>(args->code);
    delete args;

    std::cout << "=======ipc server thread======== product : " << product_type << "\n";
    char buffer_data[2048] = { 0, };

    struct pipe_info st;
    while (mgr->get_pipe_info(product_type, st))
    {
        std::cout << "=======wait======== product : " << product_type << "\n";
        int read_size = read(st.send.pipe_fd, buffer_data, sizeof(buffer_data));
        if (read_size > 0)
        {
            std::string buffer(buffer_data);
            memset(buffer_data, 0x00, 2048);

            if (!st.recv.pipe_fd)
            {
                if (named_pipe_wrapper::connect(st.recv.pipe_path, st.recv.pipe_fd, false, true))
                {
                    std::cout << "connect recv pipe success\n";
                }
                else
                {
                    std::cout << "connect recv pipe failed\n";
                }
            }
        }
        else if (read_size == 0 && st.recv.pipe_fd)
        {
            std::cout << "ipc recv pipe disconnect\n";
            named_pipe_wrapper::disconnect(st.recv.pipe_fd);

            st.recv.pipe_fd = 0;
            mgr->set_pipe_info(product_type, st);
        }
        sleep(1); //check async interval
    }
    if (st.send.pipe_fd)
    {
        named_pipe_wrapper::disconnect(st.send.pipe_fd);
        named_pipe_wrapper::remove(st.send.pipe_path);
    }
    if (st.recv.pipe_fd)
    {
        named_pipe_wrapper::disconnect(st.recv.pipe_fd);
        named_pipe_wrapper::remove(st.recv.pipe_path);
    }
    std::cout << "=======ipc server thread end======== product : " << product_type << "\n";
    pthread_exit(0);
    return NULL;
}
