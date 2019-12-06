#include "named_pipe_manager.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

enum product_list // for test
{
    EZREAL = 1,
    SORAKA = 2,
    ASHE = 4
};

int main(int argc, char * argv[])
{
    int code = product_list::EZREAL | SORAKA | ASHE ;

    bool result = named_pipe_manager::get_instance().initialize(code);
    std::cout << "init ipc result : " << result << "\n";
    while (1)
    {
        std::cout << "tick tock\n";
        sleep(1);
    }
    // for test code
    // Not Yet
}